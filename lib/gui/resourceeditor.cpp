/*
   Copyright (c) 2009-2011 Sebastian Trueg <trueg@kde.org>

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

#include "resourceeditor.h"
#include "resourceeditorannotationwidget_p.h"
#include "ui_resourceeditor.h"
#include "resourcerelationview.h"
#include "resourceannotationmodel.h"
#include "annotationdelegate.h"
#include "annotationpluginwrapper.h"
#include "annotationpluginfactory.h"
#include "annotation.h"
#include "annotationrequest.h"
#include "annotationrelevancefiltermodel.h"
#include "annotationwidget.h"

#include <klineedit.h>
#include <kdialog.h>
#include <krun.h>
#include <kicon.h>
#include <krichtextwidget.h>
#include <kactioncollection.h>
#include <kdebug.h>

#include <kratingwidget.h>
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Thing>
#include <Nepomuk/Variant>
#include <Nepomuk/Types/Class>
#include <Nepomuk/Types/Property>
#include <Nepomuk/Vocabulary/NIE>

#include <QtGui/QHBoxLayout>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QStackedWidget>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QToolButton>
#include <QtGui/QWidgetAction>
#include <QtGui/QToolBar>
#include <QtCore/QProcess>

#include <Soprano/Util/SignalCacheModel>
#include <Soprano/QueryResultIterator>
#include <Soprano/Vocabulary/NAO>

Q_DECLARE_METATYPE(Nepomuk::Annotation*)

// TODO: do not save changes right away but cache them until saved

namespace {
    QString typesToText( const QList<QUrl>& types )
    {
        QList<Nepomuk::Types::Class> typeClasses;
        foreach( const QUrl& type, types ) {
            typeClasses << Nepomuk::Types::Class( type );
        }

        // remove all types that are supertypes of others in the list
        QList<Nepomuk::Types::Class> normalizedTypes;
        for ( int i = 0; i < typeClasses.count(); ++i ) {
            Nepomuk::Types::Class& type = typeClasses[i];
            bool use = true;
            for ( int j = 0; j < typeClasses.count(); ++j ) {
                if ( type != typeClasses[j] &&
                     typeClasses[j].isSubClassOf( type ) ) {
                    use = false;
                    break;
                }
            }
            if ( use ) {
                normalizedTypes << type;
            }
        }

        // extract the labels
        QStringList typeStrings;
        for ( int i = 0; i < normalizedTypes.count(); ++i ) {
            typeStrings << normalizedTypes[i].label();
        }

        return typeStrings.join( ", " );
    }
}

class Nepomuk::ResourceEditor::Private : public Ui::ResourceEditor
{
public:
    Nepomuk::ResourceEditor* q;

    Resource m_resource;

    bool m_bRatingChanged;
    bool m_bDescriptionChanged;

    void updateWidgets();
    void _k_ratingChanged();
    void _k_descriptionChanged();
    void _k_annotationCreated(Nepomuk::Annotation* anno);
    void _k_openNepomukBrowser();
    void _k_resourceEditRequested( const Nepomuk::Resource& res );
};


void Nepomuk::ResourceEditor::Private::updateWidgets()
{
    m_labelEdit->setText( m_resource.genericLabel() );
    m_descriptionEditor->setText( m_resource.description() );
    m_ratingWidget->setRating( m_resource.rating() );
 //   m_titleLabel->setText( QString( "%1 (%2)" ).arg( m_resource.genericLabel(), typesToText( m_resource.types() ) ) );
//    m_titleLabel->setUrl( m_resource.resourceUri().toString() );

    // load relations
    m_relationView->clear();

    QHash<QUrl, Variant> properties = m_resource.properties();
    for(QHash<QUrl, Variant>::const_iterator it = properties.constBegin();
        it != properties.constEnd(); ++it) {
        const Variant& v = it.value();
        // special case files
        if(m_resource.isFile() &&
                (it.key() == Nepomuk::Vocabulary::NIE::url() ||
                 it.key() == Nepomuk::Vocabulary::NIE::isPartOf())) {
            continue;
        }
        if(v.isResource() || v.isResourceList()) {
            QList<Resource> resources = v.toResourceList();
            const Nepomuk::Types::Property p(it.key());
            Q_FOREACH(const Resource& res, resources) {
                QListWidgetItem* item = new QListWidgetItem(m_relationView);
                item->setText(QString::fromLatin1("%1: %2").arg(p.label(), res.genericLabel()));
                item->setIcon(KIcon(QLatin1String("arrow-right")));
            }
        }
    }

    // TODO: how about a "backLinks()" method in Nepomuk::Resource?
    Soprano::QueryResultIterator sit
            = ResourceManager::instance()->mainModel()->executeQuery(QString::fromLatin1("select distinct ?p ?r where { ?r ?p %1 . ?p %2 %3 . }")
                                                                     .arg(Soprano::Node::resourceToN3(m_resource.resourceUri()),
                                                                          Soprano::Node::resourceToN3(Soprano::Vocabulary::NAO::userVisible()),
                                                                          Soprano::Node::literalToN3(Soprano::LiteralValue(true))),
                                                                     Soprano::Query::QueryLanguageSparql);
    while(sit.next()) {
        const Nepomuk::Types::Property p(sit["p"].uri());
        Resource res(sit["r"].uri());
        QListWidgetItem* item = new QListWidgetItem(m_relationView);
        item->setText(QString::fromLatin1("%1: %2").arg(p.label(), res.genericLabel()));
        item->setIcon(KIcon(QLatin1String("arrow-left")));
    }

    m_bRatingChanged = false;
    m_bDescriptionChanged = false;
    m_descriptionEditor->setFocus();
}


void Nepomuk::ResourceEditor::Private::_k_ratingChanged()
{
    m_bRatingChanged = true;
    emit q->annotationsChanged();
}

void Nepomuk::ResourceEditor::Private::_k_descriptionChanged()
{
    m_bDescriptionChanged = true;
    emit q->annotationsChanged();
}

void Nepomuk::ResourceEditor::Private::_k_annotationCreated(Annotation* anno)
{
    QListWidgetItem* item = new QListWidgetItem(m_relationView);
    item->setText(anno->comment());
    item->setIcon(KIcon(QLatin1String("arrow-right")));
    item->setData(Qt::UserRole, QVariant::fromValue(anno));
}

// TODO: emit a signal instead of doing this here
void Nepomuk::ResourceEditor::Private::_k_resourceEditRequested( const Nepomuk::Resource& res )
{
    Nepomuk::Thing thing = Resource( res ).pimoThing();
    KDialog dlg( q );
    Nepomuk::ResourceEditor* re = new Nepomuk::ResourceEditor( &dlg );
    re->setResource( thing );
    dlg.setMainWidget( re );
    dlg.setButtons( KDialog::Close );
    dlg.setCaption( i18n( "Editing Resource %1", thing.genericLabel() ) );
    dlg.exec();
}


// TODO: emit a signal instead of handling it here.
void Nepomuk::ResourceEditor::Private::_k_openNepomukBrowser()
{
    QProcess::startDetached( "kde-open", QStringList() << m_resource.resourceUri().toString() );
}


Nepomuk::ResourceEditor::ResourceEditor( QWidget* parent )
    : QWidget( parent ),
      d( new Private() )
{
    // setup ui
    d->q = this;
    d->setupUi( this );

    // create the action buttons
    QHBoxLayout* toolBarLayout = new QHBoxLayout(d->m_toolBar);
    toolBarLayout->setMargin(0);
    QToolBar* toolBar = new QToolBar(d->m_toolBar);
    toolBar->setIconSize(QSize(16, 16));
    toolBarLayout->addWidget(toolBar);
    KActionCollection* actionCollection = new KActionCollection(this);
    d->m_descriptionEditor->createActions(actionCollection);
    Q_FOREACH(QAction* action, actionCollection->actions()) {

        // Hack to exclude some actions
        if(action->objectName() == "direction_ltr" ||
                action->objectName() == "direction_rtl" ||
                action->objectName() == "manage_link" ||
                action->objectName() == "action_to_plain_text")
            continue;

        toolBar->addAction(action);
    }

    // connect the gui
    connect(d->m_labelEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(annotationsChanged()));
    connect(d->m_descriptionEditor, SIGNAL(textChanged()),
            this, SLOT(_k_descriptionChanged()));
    connect( d->m_ratingWidget, SIGNAL( ratingChanged( int ) ),
             this, SLOT(_k_ratingChanged()) );

    connect(d->m_annotationWidget, SIGNAL(annotationCreated(Nepomuk::Annotation*)),
            this, SLOT(_k_annotationCreated(Nepomuk::Annotation*)));
}


Nepomuk::ResourceEditor::~ResourceEditor()
{
    delete d;
}


Nepomuk::Resource Nepomuk::ResourceEditor::resource() const
{
    return d->m_resource;
}


void Nepomuk::ResourceEditor::setResource( const Nepomuk::Resource& res,
                                           const QList<Nepomuk::Annotation*>& manualAnnotations )
{
    d->m_resource = res;
    d->m_annotationWidget->setResource( res );
    d->updateWidgets();
}


bool Nepomuk::ResourceEditor::eventFilter( QObject* obj, QEvent* event )
{
#if 0
    if ( obj == d->m_relationView ) {
        if ( event->type() == QEvent::DragEnter ) {
            QDragEnterEvent* de = static_cast<QDragEnterEvent*>( event );
            event->setAccepted( de->mimeData()->hasFormat( d->m_annotationModel->mimeTypes()[0] ) );
            return true;
        }
        else if ( event->type() == QEvent::Drop ) {
            QDropEvent* de = static_cast<QDragEnterEvent*>( event );
            if ( de->mimeData()->hasFormat( d->m_annotationModel->mimeTypes()[0] ) ) {
                QModelIndexList indices = d->m_annotationModel->indexListFromMimeData( de->mimeData() );
                foreach( const QModelIndex& index, indices ) {
                    Annotation* anno = d->m_annotationModel->annotationForIndex( index );
                    anno->create( d->m_resource );
                    delete anno;
                }
                return true;
            }
        }
    }
#endif
    return QWidget::eventFilter( obj, event );
}

void Nepomuk::ResourceEditor::save()
{
    if(d->m_bRatingChanged) {
        d->m_resource.setRating(d->m_ratingWidget->rating());
    }
    if(d->m_labelEdit->isModified()) {
        d->m_resource.setLabel(d->m_labelEdit->text());
    }
    if(d->m_bDescriptionChanged) {
        d->m_resource.setDescription(d->m_descriptionEditor->toHtml());
    }

    // apply annotations
    for(int i = 0; i < d->m_relationView->count(); ++i) {
        QListWidgetItem* item = d->m_relationView->item(i);
        const QVariant av = item->data(Qt::UserRole);
        if(av.isValid()) {
            av.value<Nepomuk::Annotation*>()->create(d->m_resource);
        }
    }

    d->updateWidgets();
}

#include "resourceeditor.moc"
