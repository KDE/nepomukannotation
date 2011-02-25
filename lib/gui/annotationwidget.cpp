/*
   Copyright (c) 2008 Sebastian Trueg <trueg@kde.org>

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

#include "annotationwidget.h"
#include "resourceannotationmodel.h"
#include "annotation.h"
#include "annotationrelevancefiltermodel.h"
#include <kpixmapsequenceoverlaypainter.h>
#include <kpixmapsequence.h>

#include <QtGui/QListView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QScrollBar>
#include <QtCore/QTimer>

#include <klineedit.h>
#include <kdebug.h>
#include <klocale.h>
#include <kicon.h>

Q_DECLARE_METATYPE( Nepomuk::Annotation* )


class Nepomuk::AnnotationWidget::Private
{
public:
    QListView* m_annotationsView;
    KLineEdit* m_filterEdit;

    Nepomuk::ResourceAnnotationModel* m_model;
    Nepomuk::AnnotationRelevanceFilterModel* m_sortModel;

    // true if the user navigated the list already and we do not
    // want to confuse them with resetting the selection
    bool navigationPerformed;

    KPixmapSequenceOverlayPainter* m_busyPainter;

    void _k_slotAnnotationFinished( Nepomuk::Annotation* anno );
    void _k_slotExecuteAnnotation( Nepomuk::Annotation* anno );
    void _k_slotFilterChanged( const QString& filter );
    void _k_slotAnnotationsChanged();

    void executeAnnotationAtIndex( const QModelIndex& index );

    AnnotationWidget* q;
};



void Nepomuk::AnnotationWidget::Private::_k_slotAnnotationFinished( Nepomuk::Annotation* anno )
{
    kDebug() << "Annotation" << anno->label() << "done";
    emit q->annotationAdded( m_model->resource() );
}


void Nepomuk::AnnotationWidget::Private::_k_slotFilterChanged( const QString& filter )
{
    kDebug() << filter;
    navigationPerformed = false;
    m_busyPainter->start();
    m_model->setFilter( filter );
}


void Nepomuk::AnnotationWidget::Private::_k_slotAnnotationsChanged()
{
    m_sortModel->sort(0, Qt::DescendingOrder);
    if ( !navigationPerformed )
        m_annotationsView->selectionModel()->select( m_sortModel->index( 0, 0 ),
                                                     QItemSelectionModel::Clear|QItemSelectionModel::SelectCurrent );
}


void Nepomuk::AnnotationWidget::Private::_k_slotExecuteAnnotation( Nepomuk::Annotation* anno )
{
    kDebug() << "Creating annotation" << anno;
    q->createAnnotation( anno );
}


void Nepomuk::AnnotationWidget::Private::executeAnnotationAtIndex( const QModelIndex& index )
{
    // take the annotation from the list
    Nepomuk::Annotation* anno = m_model->takeAnnotation( m_sortModel->mapToSource( index ) );
    connect( anno, SIGNAL( finished( Nepomuk::Annotation* ) ),
             q, SLOT( _k_slotAnnotationFinished( Nepomuk::Annotation* ) ) );

    // create the annotation async
    kDebug() << "Creating annotation" << anno;
    QMetaObject::invokeMethod( q, "_k_slotExecuteAnnotation", Qt::QueuedConnection, Q_ARG( Nepomuk::Annotation*, anno ) );

    // select the next in the list
    int rc = m_sortModel->rowCount();
    if ( rc ) {
        m_annotationsView->selectionModel()->select( m_sortModel->index( index.row() < rc ? index.row() : rc-1, 0 ),
                                                     QItemSelectionModel::Clear|QItemSelectionModel::Select );
    }
}


Nepomuk::AnnotationWidget::AnnotationWidget( QWidget* parent )
    : QWidget( parent ),
      d( new Private() )
{
    d->q = this;

    d->navigationPerformed = false;

    d->m_filterEdit = new KLineEdit( this );

    d->m_busyPainter = new KPixmapSequenceOverlayPainter( this );
    d->m_busyPainter->setWidget( d->m_filterEdit );
    d->m_busyPainter->setAlignment( Qt::AlignRight|Qt::AlignVCenter );

    d->m_model = new Nepomuk::ResourceAnnotationModel( d->m_annotationsView );
    d->m_sortModel = new AnnotationRelevanceFilterModel( d->m_model );
    d->m_sortModel->setSourceModel( d->m_model );
    d->m_sortModel->setSortRole( Nepomuk::AnnotationModel::RelevanceRole );
    d->m_sortModel->setMinRelevance(0.1);

    d->m_annotationsView = new QListView( this );
    d->m_annotationsView->setModel( d->m_sortModel );
    d->m_annotationsView->setFocusPolicy(Qt::NoFocus);
    d->m_annotationsView->setSelectionMode( QAbstractItemView::SingleSelection );

    QVBoxLayout* lay = new QVBoxLayout( this );
    lay->setMargin( 0 );
    lay->addWidget( d->m_filterEdit );
    lay->addWidget( d->m_annotationsView );

    d->m_filterEdit->installEventFilter( this );
    d->m_annotationsView->installEventFilter( this );
    d->m_annotationsView->viewport()->installEventFilter( this );

    connect( d->m_filterEdit, SIGNAL( textEdited( QString ) ),
             this, SLOT( _k_slotFilterChanged( QString ) ) );
    connect( d->m_model, SIGNAL( rowsInserted( const QModelIndex&, int, int ) ),
             this, SLOT( _k_slotAnnotationsChanged() ) );

    connect( d->m_model, SIGNAL( finished() ),
             d->m_busyPainter, SLOT( stop() ) );

    d->m_filterEdit->setClickMessage( i18nc( "The user can enter this line edit to annotate a resource. "
                                            "This means to add meta data such as tags or relations to other resources.",
                                            "Annotate..." ) );

    qRegisterMetaType<Nepomuk::Annotation*>();
}


Nepomuk::AnnotationWidget::~AnnotationWidget()
{
    delete d;
}


bool Nepomuk::AnnotationWidget::eventFilter( QObject* watched, QEvent* event )
{
    switch( event->type() ) {
    case QEvent::KeyPress: {
        QKeyEvent* ke = static_cast<QKeyEvent*>( event );
        switch( ke->key() ) {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            if(!d->navigationPerformed || watched == d->m_filterEdit) {
                d->navigationPerformed = true;
                QApplication::sendEvent(d->m_annotationsView, event);
                return true;
            }
            else {
                return false;
            }

        case Qt::Key_Return:
        case Qt::Key_Enter: {
            if ( d->m_annotationsView->selectionModel()->hasSelection() ) {
                d->executeAnnotationAtIndex( d->m_annotationsView->selectionModel()->selectedIndexes().first() );
            }
            return true;
        }

        case Qt::Key_Escape:
            if(!d->m_filterEdit->text().isEmpty()) {
                d->m_filterEdit->clear();
                d->_k_slotFilterChanged(QString());
                return true;
            }
            else {
                // hitting ESC twice should exit a dialog or whatever
                return false;
            }

        default:
            (static_cast<QObject *>(d->m_filterEdit))->event(ke);
            return true;
        }
        break;
    }

    case QEvent::MouseButtonRelease: {
        QMouseEvent* me = static_cast<QMouseEvent*>( event );
        if ( d->m_annotationsView->isVisible() &&
             !d->m_filterEdit->rect().contains( d->m_filterEdit->mapFromGlobal( me->globalPos() ) ) &&
            !d->m_annotationsView->rect().contains( d->m_annotationsView->mapFromGlobal( me->globalPos() ) ) ) {
            d->m_annotationsView->hide();
        }
        break;
    }

    case QEvent::MouseButtonDblClick: {
        QMouseEvent* me = static_cast<QMouseEvent*>( event );
        if ( me->button() == Qt::LeftButton ) {
            if ( d->m_annotationsView->isVisible() ) {
                QPoint pos = d->m_annotationsView->viewport()->mapFromGlobal( me->globalPos() );
                QModelIndex index = d->m_annotationsView->indexAt( pos );
                if ( index.isValid() ) {
                    d->executeAnnotationAtIndex( index );
                    return true;
                }
            }

        }
        break;
    }

    default:
        return false;
    }
    return false;
}


void Nepomuk::AnnotationWidget::setResource( const Resource& resource )
{
    d->m_model->setResource( resource );

    // FIXME: use a custom filter model or some other means to support AnnotationWidget subclasses
    d->m_sortModel->setFilterResource( resource );

    d->m_filterEdit->clear();
}


Nepomuk::Resource Nepomuk::AnnotationWidget::resource() const
{
    return d->m_model->resource();
}


void Nepomuk::AnnotationWidget::createAnnotation( Nepomuk::Annotation* anno )
{
    kDebug() << "Creating annotation" << anno;
    anno->create( d->m_model->resource() );
}

#include "annotationwidget.moc"
