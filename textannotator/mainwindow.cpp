/*
   Copyright (c) 2008-2011 Sebastian Trueg <trueg@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "mainwindow.h"
#include "textannotationwidget.h"
#include "annotationdisplay.h"
#include "annotationmodel.h"
#include "annotationdelegate.h"
#include "annotationpluginwrapper.h"
#include "annotationpluginfactory.h"
#include "annotation.h"
#include "textannotation.h"
#include "annotationrequest.h"
#include "annotationrelevancefiltermodel.h"
#include "textextractionjob.h"
#include "nfo.h"
#include "nie.h"
#include "statementeditor.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QTextCursor>
#include <QtGui/QStatusBar>
#include <QtGui/QWidgetAction>
#include <QtGui/QContextMenuEvent>
#include <QtCore/QFile>

#include <Nepomuk/Thing>
#include <Nepomuk/Variant>
#include <Nepomuk/Types/Property>
#include <Nepomuk/ResourceManager>

#include <Soprano/Soprano>

#include <KLocale>
#include <KDebug>
#include <KMenu>

Q_DECLARE_METATYPE(Nepomuk::Annotation*)


namespace {
    QTextEdit::ExtraSelection createSelectionsForAnnotation( Nepomuk::TextAnnotation* anno, QTextDocument* doc )
    {
        kDebug() << "Creating selection for" << anno << anno->textOffset() << anno->textLength();
        QTextEdit::ExtraSelection selection;
        selection.cursor = QTextCursor( doc );
        selection.cursor.setPosition( anno->textOffset() );
        selection.cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor, anno->textLength() );
        selection.format.setBackground( Qt::green );
        return selection;
    }

    QList<Nepomuk::Resource> getTextExcerpts( const Nepomuk::Resource& textRes )
    {
        // get all parts that are TextDocuments
        QList<Nepomuk::Resource> parts = textRes.property( Nepomuk::Vocabulary::NIE::hasLogicalPart() ).toResourceList();
        QList<Nepomuk::Resource>::iterator it = parts.begin();
        while ( it != parts.end() ) {
            if ( it->hasType( Nepomuk::Vocabulary::NFO::TextDocument() ) )
                ++it;
            else
                it = parts.erase( it );
        }
        return parts;
    }

    QList<QTextEdit::ExtraSelection> createSelectionsForResource( const Nepomuk::Resource& res, QTextDocument* doc )
    {
        // extract all excerpts
        QList<QTextEdit::ExtraSelection> selections;
        foreach( const Nepomuk::Resource& tr, getTextExcerpts( res ) ) {
            QTextEdit::ExtraSelection selection;
            selection.cursor = QTextCursor( doc );
            selection.cursor.setPosition( tr.property( QUrl(Nepomuk::Vocabulary::NFO::nfoNamespace().toString() + "textOffset") ).toInt() );
            selection.cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor, tr.property( Nepomuk::Vocabulary::NFO::characterCount() ).toInt() );
            selection.format.setBackground( Qt::yellow );
            selections << selection;
        }

        return selections;
    }

    Nepomuk::Resource findTextResourcePart( const Nepomuk::Resource& res, int textPos )
    {
        foreach( const Nepomuk::Resource& tr, getTextExcerpts( res ) ) {
            int pos = tr.property( QUrl(Nepomuk::Vocabulary::NFO::nfoNamespace().toString() + "textOffset") ).toInt();
            int len = tr.property( Nepomuk::Vocabulary::NFO::characterCount() ).toInt();
            if ( textPos >= pos && textPos <= pos + len )
                return tr;
        }

        // nothing found
        return Nepomuk::Resource();
    }
}


MainWindow::MainWindow( const KUrl& res )
    : QMainWindow(),
      m_textExtractionJob(0)
{
    // setup UI
    setWindowTitle( i18n( "Nepomuk - Annotate Resource" ) );
    QWidget* w = new QWidget( this );
    setupUi( w );
    setCentralWidget( w );

    // configure UI
    m_textBrowser->viewport()->installEventFilter( this );
    m_annotationDisplay->setNavigationEnabled( true );
    m_annotationDisplay->setPropertyRemovalEnabled( true );
    connect( m_annotationDisplay, SIGNAL( displayedResourceChanged( Nepomuk::Resource ) ),
             this, SLOT( setResource( Nepomuk::Resource ) ) );

    m_annotationModel = new Nepomuk::AnnotationModel( m_annotationView );
    m_filterModel = new Nepomuk::AnnotationRelevanceFilterModel( m_annotationView );
    m_filterModel->setMinRelevance( 0.01 );
    m_filterModel->setSortRole( Nepomuk::AnnotationModel::RelevanceRole );
    m_filterModel->setDynamicSortFilter( true );
    m_filterModel->setSourceModel( m_annotationModel );
    m_annotationView->setModel( m_filterModel );
    m_delegate = new Nepomuk::AnnotationDelegate( m_annotationView );
    m_annotationView->setItemDelegate( m_delegate );
    connect( m_annotationView->selectionModel(), SIGNAL( currentChanged(const QModelIndex&, const QModelIndex&) ),
             this, SLOT( slotSelectionChanged(const QModelIndex&, const QModelIndex&) ) );

    Nepomuk::AnnotationPluginFactory* factory = new Nepomuk::AnnotationPluginFactory( this );
    m_annotator = new Nepomuk::AnnotationPluginWrapper( this );
    m_annotator->setPlugins( factory->getAllPlugins() );
    connect( m_annotator, SIGNAL( newAnnotation( Nepomuk::Annotation* ) ),
             m_annotationModel, SLOT( addNewAnnotation( Nepomuk::Annotation* ) ) );
    connect( m_annotator, SIGNAL( finished() ),
             this, SLOT( slotAnotatorFinished() ) );

    m_relationView->hide();
    m_annotationDisplay->setHiddenProperties( QList<Nepomuk::Types::Property>()
                                              << Nepomuk::Types::Property( Nepomuk::Vocabulary::NIE::plainTextContent() )
                                              << Nepomuk::Types::Property( Soprano::Vocabulary::Xesam::asText() ) );

    // setup resource
    setResource( Nepomuk::Resource( res ) );
}


MainWindow::~MainWindow()
{
}


void MainWindow::slotTextExtractionResult( KJob* )
{
    QString text = m_textExtractionJob->text();
    m_textExtractionJob = 0;
    if ( !text.isEmpty() ) {
        kDebug() << "Starting annotator";

        m_textBrowser->setPlainText( text );
        m_textBrowser->show();
        m_annotationView->show();
        m_textBrowser->disconnect( this );
        connect( m_textBrowser, SIGNAL( cursorPositionChanged() ),
                 this, SLOT( slotCursorPositionChanged() ) );

        // fire up the annotation system
        Nepomuk::AnnotationRequest request;
#warning FIXME: nothing is really done with the text
        //request.setText( text );
        m_annotator->getPossibleAnnotations( request );

        slotSelectionChanged( QModelIndex(), QModelIndex() );
    }
}


void MainWindow::slotAnotatorFinished()
{
    kDebug() << "Annotator finished";
}


void MainWindow::slotSelectionChanged( const QModelIndex& current, const QModelIndex& )
{
    if ( Nepomuk::TextAnnotation* anno
         = qobject_cast<Nepomuk::TextAnnotation*>( current.data( Nepomuk::AnnotationModel::AnnotationRole ).value<Nepomuk::Annotation*>() ) ) {
        m_textBrowser->setExtraSelections( QList<QTextEdit::ExtraSelection>() << createSelectionsForAnnotation( anno, m_textBrowser->document() ) );

    }
    else {
        // mark them all
        QList<QTextEdit::ExtraSelection> selections;
        for ( int i = 0; i < m_annotationModel->rowCount(); ++i ) {
            if ( Nepomuk::TextAnnotation* anno
                 = qobject_cast<Nepomuk::TextAnnotation*>( m_annotationModel->index( i, 0 ).data( Nepomuk::AnnotationModel::AnnotationRole ).value<Nepomuk::Annotation*>() ) )
                selections << createSelectionsForAnnotation( anno, m_textBrowser->document() );
        }
        m_textBrowser->setExtraSelections( selections + createSelectionsForResource( m_annotationWidget->resource(), m_textBrowser->document() ) );
    }
}


void MainWindow::slotCursorPositionChanged()
{
    QTextCursor cursor = m_textBrowser->textCursor();
    if ( cursor.hasSelection() ) {
        m_annotationWidget->setTextExcept( cursor.selectionStart(), cursor.selectedText().length(), cursor.selectedText() );
        statusBar()->showMessage( i18n( "Annotating text excerpt at position %1 ('%2')",
                                        cursor.selectionStart(),
                                        cursor.selectedText() ) );
    }
    else {
        m_annotationWidget->resetTextExcerpt();
        statusBar()->showMessage( i18n( "Annotating the full text" ) );
    }
}


void MainWindow::setResource( Nepomuk::Resource resource )
{
    m_annotationView->hide();
    m_textBrowser->hide();
    m_textBrowser->clear();

    m_annotationWidget->setResource( resource );
    m_annotationDisplay->setResource( resource );

    m_delegate->setResource( resource );
    m_filterModel->setFilterResource( resource );

    m_relationView->setResource( resource );

    m_statementEditor->setResource( resource );

    if ( QFile::exists( resource.resourceUri().toLocalFile() ) ) {
        // fire up the text analysis
        if ( m_textExtractionJob )
            m_textExtractionJob->disconnect( this );
        m_textExtractionJob = new Nepomuk::TextExtractionJob();
        connect( m_textExtractionJob, SIGNAL( result( KJob* ) ),
                 this, SLOT( slotTextExtractionResult( KJob* ) ) );
        m_textExtractionJob->setResource( resource );
        m_textExtractionJob->start();

        slotCursorPositionChanged();
    }
}


namespace {
    QWidget* createTextInfoWidget( const Nepomuk::Resource& res )
    {
        Nepomuk::AnnotationDisplay* w = new Nepomuk::AnnotationDisplay();
        w->setResource( res );
        return w;
    }
}

bool MainWindow::eventFilter( QObject* watched, QEvent* event )
{
    if ( watched == m_textBrowser->viewport() &&
         event->type() == QEvent::ContextMenu ) {
        QContextMenuEvent* ce = static_cast<QContextMenuEvent*>( event );
        kDebug() << ce->pos();
        int textPos = m_textBrowser->cursorForPosition( m_textBrowser->viewport()->mapFrom( m_textBrowser, ce->pos() ) ).position();
        kDebug() << textPos;
        Nepomuk::Resource textRes = findTextResourcePart( m_annotationWidget->resource(), textPos );
        if ( textRes.isValid() ) {
            KMenu popupMenu;
            QWidgetAction* action = new QWidgetAction( &popupMenu );
            action->setDefaultWidget( createTextInfoWidget( textRes ) );
            popupMenu.addAction( action );
            popupMenu.exec( ce->globalPos() );
            return true;
        }
    }

    return QMainWindow::eventFilter( watched, event );
}
