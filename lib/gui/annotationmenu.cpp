/*
   This file is part of the Nepomuk KDE project.
   Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) version 3, or any
   later version accepted by the membership of KDE e.V. (or its
   successor approved by the membership of KDE e.V.), which shall
   act as a proxy defined in Section 6 of version 3 of the license.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "annotationmenu.h"
#include "searchmenu.h"
#include "newresourcedialog.h"
#include "pimo.h"
#include "nuao.h"
#include "tmo.h"
#include "nepomukcontext.h"
#include "resourceannotationmodel.h"
#include "annotation.h"
#include "kedittagsdialog_p.h"

#include <QtCore/QPointer>
#include <QtCore/QList>
#include <QtCore/QProcess>
#include <QtGui/QAction>
#include <QtGui/QLabel>

#include <KMenu>
#include <KLocale>
#include <KIcon>
#include <KActionMenu>
#include <KLineEdit>
#include <KDebug>
#include <KStandardDirs>

#include <Nepomuk/KRatingWidget>
#include <Nepomuk/Tag>
#include <Nepomuk/Thing>
#include <Nepomuk/Variant>
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Query/Query>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/NegationTerm>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/OptionalTerm>
#include <Nepomuk/Types/Class>
#include <Nepomuk/Types/Property>
#include <nepomuk/nepomukmassupdatejob.h>

#include <Soprano/Model>
#include <Soprano/Node>
#include <Soprano/QueryResultIterator>
#include <Soprano/Util/AsyncQuery>
#include <Soprano/Vocabulary/NAO>

// layout with resoruce:
//
// ------------------
// - resource label -
// ------------------
// Rating: *****
// ------------------
// Relate to:
// res1
// res2
// res3
// res4
// res5
// more...
// ------------------
// New Task...
// New Event...
// New Person...
// ------------------
//
//
// layout without resource:
//
// ------------------
// New Task...
// New Event...
// New Person...
// ------------------
// Set current context:
// res1
// res2
// res3
// res4
// res5
// more...
// ------------------
//

// TODO: most of the "related resources" and "set context" action methods are similar and could be merged
// TODO: It would be great to not only sort tags and resources by usage count but also by creation date, i.e. recently created get a higher prio
//       The only question is: how to merge this with use count and can it be done in a single query??

Q_DECLARE_METATYPE( Nepomuk::Types::Class )
Q_DECLARE_METATYPE( Nepomuk::Annotation* )
Q_DECLARE_METATYPE( KLineEdit* )

static const int s_maxActionsPerSection = 5;

class Nepomuk::AnnotationMenu::Private
{
public:
    Private()
        : m_configFlags( AnnotationMenu::ShowAll ),
          m_annotationTitleCreated( false ),
          m_searchMenu( 0 ),
          m_annotationModel( 0 ),
          m_menuCreated( false ) {
    }

    typedef QPair<Nepomuk::Types::Class, Nepomuk::Query::Term> TypeElement;

    void clear();

    QList<TypeElement> resourceTypes() const;

    Query::Term createResourceTypeTerm() const;

    QList<QAction*> newResourceActions( KMenu* menu );
    QAction* createRelateToAction( const QUrl& resource ) const;
    QAction* createContextAction( const QUrl& resource ) const;
    QAction* createAnnotationAction( Annotation* annotation ) const;
    QAction* createTagAction( const Nepomuk::Tag& tag ) const;
    QAction* createShowAllTagsAction() const;

    void addNewResourceActions( KMenu* menu );
    void addRatingAction( KMenu* menu );
    void addRelateToActions( KMenu* menu );
    void addContextActions( KMenu* menu );
    void addAnnotationActions( KMenu* menu );
    void addTagActions( KMenu* menu );
    void addSearchAction( KMenu* menu );
    void addOpenWithAction( KMenu* menu );

    void _k_menuAboutToShow();
    void _k_ratingChanged(unsigned int);
    void _k_relateToActionTriggered();
    void _k_contextActionTriggered();
    void _k_newResourceActionTriggered();
    void _k_annotationActionTriggered();
    void _k_searchResultTriggered( const Nepomuk::Query::Result& );
    void _k_tagActionToggled(bool);
    void _k_openWithActionTriggered();
    void _k_nextRelateToResourceReady( Soprano::Util::AsyncQuery* query );
    void _k_relateToResourceQueryFinished( Soprano::Util::AsyncQuery* );
    void _k_nextContextReady( Soprano::Util::AsyncQuery* );
    void _k_nextTagReady( Soprano::Util::AsyncQuery* );
    void _k_tagQueryFinished( Soprano::Util::AsyncQuery* );
    void _k_contextQueryFinished( Soprano::Util::AsyncQuery* );
    void _k_newAnnotation( Nepomuk::Annotation* );
    void _k_annotationModelFinished();
    void _k_showAllTags();

    void populateAction( QAction* action, const Nepomuk::Resource& res, bool withTypeLabel = true ) const;

    AnnotationMenu::ConfigurationFlags m_configFlags;

    QList<TypeElement> m_resourceTypes;

    QList<Resource> m_resources;

    QPointer<QAction> m_relateToBusyAction;
    QPointer<QAction> m_contextQueryBusyAction;
    QPointer<QAction> m_annotationsBusyAction;
    QPointer<QAction> m_tagBusyAction;
    bool m_annotationTitleCreated;

    QPointer<QMenu> m_tagActionMenu;
    QPointer<QMenu> m_relateToActionMenu;
    QPointer<QMenu> m_contextActionMenu;

    int m_tagActionCount;
    int m_annotationActionCount;

    SearchMenu* m_searchMenu;
    ResourceAnnotationModel* m_annotationModel;

    // true if actions have been created already.
    bool m_menuCreated;

    AnnotationMenu* q;
};


void Nepomuk::AnnotationMenu::Private::clear()
{
    q->clear();
    m_menuCreated = false;
}


QList<Nepomuk::AnnotationMenu::Private::TypeElement> Nepomuk::AnnotationMenu::Private::resourceTypes() const
{
    QList<TypeElement> types = m_resourceTypes;
    if ( types.isEmpty() ) {
        // add default resource types
        types.append( qMakePair( Types::Class( Nepomuk::Vocabulary::PIMO::Project() ), Query::Term() ) );
        types.append( qMakePair( Types::Class( Nepomuk::Vocabulary::PIMO::Task() ),
                                 Query::Term(
                                     Query::AndTerm(
                                         Query::ResourceTypeTerm( Nepomuk::Vocabulary::PIMO::Task() ),
                                         Query::NegationTerm::negateTerm(
                                             Query::OrTerm(
                                                 Query::ComparisonTerm(
                                                     Nepomuk::Vocabulary::TMO::taskState(),
                                                     Query::ResourceTerm( Nepomuk::Vocabulary::TMO::TMO_Instance_TaskState_Completed() ) ),
                                                 Query::ComparisonTerm(
                                                     Nepomuk::Vocabulary::TMO::taskState(),
                                                     Query::ResourceTerm( Nepomuk::Vocabulary::TMO::TMO_Instance_TaskState_Archived() ) ),
                                                 Query::ComparisonTerm(
                                                     Nepomuk::Vocabulary::TMO::taskState(),
                                                     Query::ResourceTerm( Nepomuk::Vocabulary::TMO::TMO_Instance_TaskState_Deleted() ) ),
                                                 Query::ComparisonTerm(
                                                     Nepomuk::Vocabulary::TMO::taskState(),
                                                     Query::ResourceTerm( Nepomuk::Vocabulary::TMO::TMO_Instance_TaskState_Finalized() ) ),
                                                 Query::ComparisonTerm(
                                                     Nepomuk::Vocabulary::TMO::taskState(),
                                                     Query::ResourceTerm( Nepomuk::Vocabulary::TMO::TMO_Instance_TaskState_Terminated() ) ) ) ) ) ) ) );
        types.append( qMakePair( Types::Class( Nepomuk::Vocabulary::PIMO::Person() ), Query::Term() ) );
    }
    return types;
}


Nepomuk::Query::Term Nepomuk::AnnotationMenu::Private::createResourceTypeTerm() const
{
    Query::OrTerm typeOr;
    Q_FOREACH( const TypeElement& elem, resourceTypes() ) {
        const Types::Class& type = elem.first;
        const Query::Term& term = elem.second;
        if ( !term.isValid() )
            typeOr.addSubTerm( Query::ResourceTypeTerm( type ) );
        else
            typeOr.addSubTerm( term );
    }
    return typeOr;
}


QList<QAction*> Nepomuk::AnnotationMenu::Private::newResourceActions( KMenu* menu )
{
    kDebug();
    QList<QAction*> actions;
    Q_FOREACH( const TypeElement& type, resourceTypes() ) {
        QAction* action = new QAction( menu );
        action->setText( i18nc( "@menu:action Action to trigger a dlg which will allow to create a new resource of type %1",
                                "New %1...",
                                type.first.label() ) );
        action->setData( QVariant::fromValue( type.first ) );
        q->connect( action, SIGNAL( triggered() ), SLOT( _k_newResourceActionTriggered() ) );
        actions << action;
    }
    return actions;
}


QAction* Nepomuk::AnnotationMenu::Private::createRelateToAction( const QUrl& resource ) const
{
    kDebug() << resource;
    Nepomuk::Resource res( resource );
    QAction* action = new QAction( m_relateToActionMenu );
    populateAction( action, res );
    q->connect( action, SIGNAL( triggered() ), SLOT( _k_relateToActionTriggered() ) );
    return action;
}


QAction* Nepomuk::AnnotationMenu::Private::createContextAction( const QUrl& resource ) const
{
    kDebug() << resource;
    Nepomuk::Resource res( resource );
    QAction* action = new QAction( m_contextActionMenu );
    populateAction( action, res );
    q->connect( action, SIGNAL( triggered() ), SLOT( _k_contextActionTriggered() ) );
    return action;
}


QAction* Nepomuk::AnnotationMenu::Private::createAnnotationAction( Annotation* annotation ) const
{
    kDebug();
    QAction* action = new QAction( q );
    q->setElidedActionText( action, annotation->comment() );
    action->setData( QVariant::fromValue( annotation ) );
    q->connect( action, SIGNAL( triggered() ), SLOT(_k_annotationActionTriggered()) );
    return action;
}


namespace {
    bool allResourcesTaggedWith( const QList<Nepomuk::Resource>& resources, const Nepomuk::Tag& tag ) {
        Q_FOREACH( const Nepomuk::Resource& res, resources ) {
            if ( !res.tags().contains( tag ) )
                return false;
        }
        return true;
    }
}

QAction* Nepomuk::AnnotationMenu::Private::createTagAction( const Nepomuk::Tag& tag ) const
{
    kDebug() << tag;
    QAction* action = new QAction( m_tagActionMenu );
    action->setCheckable(true);
    populateAction( action, tag, false );
    if ( allResourcesTaggedWith( m_resources, tag ) ) {
        action->setChecked( true );
    }
    q->connect( action, SIGNAL(toggled(bool)), SLOT(_k_tagActionToggled(bool)) );
    return action;
}


QAction* Nepomuk::AnnotationMenu::Private::createShowAllTagsAction() const
{
    kDebug();
    QAction* action = new QAction( m_tagActionMenu );
    action->setText( i18nc( "@action:inmenu", "Show all tags..." ) );
    q->connect( action, SIGNAL( triggered() ), SLOT( _k_showAllTags() ) );
    return action;
}


namespace {
    /// return the rating if it is the same for all resources, otherwise return 0
    int mergeRating( const QList<Nepomuk::Resource>& resources ) {
        int rating = 0;
        Q_FOREACH( const Nepomuk::Resource& res, resources ) {
            int r = res.rating();
            if ( rating == 0 ||
                 r == rating )
                rating = r;
            else
                return 0;
        }
        return rating;
    }
}

void Nepomuk::AnnotationMenu::Private::addRatingAction( KMenu* menu )
{
    kDebug();
	QWidgetAction* rateAction = new QWidgetAction( menu );
	KRatingWidget* ratingWidget = new KRatingWidget();
    ratingWidget->setRating( mergeRating( m_resources ) );
	rateAction->setDefaultWidget( ratingWidget );
	connect( ratingWidget, SIGNAL(ratingChanged(unsigned int)),
             q, SLOT(_k_ratingChanged(unsigned int)) );
    menu->addAction( rateAction );
}


void Nepomuk::AnnotationMenu::Private::addNewResourceActions( KMenu* menu )
{
    kDebug();
    QList<QAction*> newActions = newResourceActions( menu );
    if ( newActions.count() > 3 ) {
        KActionMenu* newSubMenu = new KActionMenu( menu );
        newSubMenu->setText( i18nc( "@action Label for a submenu containing a set of actions to create new resources.", "New" ) );
        Q_FOREACH( QAction* a, newActions )
            newSubMenu->addAction( a );
        menu->addAction( newSubMenu );
    }
    else {
        Q_FOREACH( QAction* a, newActions )
            menu->addAction( a );
    }
}


void Nepomuk::AnnotationMenu::Private::addRelateToActions( KMenu* menu )
{
    kDebug();

    m_relateToActionMenu = menu;

    // add widget action showing a busy thingi and query the resources async
    m_relateToBusyAction = q->createBusyAction(menu);
    menu->addAction( m_relateToBusyAction );

    // query: select the resources that are not already related to m_resource
    //        and that have one of the configured types
    //        and that have a high usage count
    Query::AndTerm mainTerm;

    // restrict to one of the supported resource types
    mainTerm.addSubTerm( createResourceTypeTerm() );

    // make sure we ignore resources that are already related to all our resources
    Query::AndTerm ignoreRelated;
    Q_FOREACH( const Resource& res, m_resources )
        if ( res.exists() ) {
            ignoreRelated.addSubTerm(
                Query::ComparisonTerm( Types::Property(), Query::ResourceTerm( res ) ).inverted()
                );
        }
    if ( !ignoreRelated.subTerms().isEmpty() )
        mainTerm.addSubTerm( Query::NegationTerm::negateTerm( ignoreRelated ) );

    // sort the resources by most often used
    Query::ComparisonTerm ct;
    ct.setAggregateFunction( Query::ComparisonTerm::DistinctCount );
    ct.setSortWeight( 1, Qt::DescendingOrder );
    mainTerm.addSubTerm( ct.inverted() );

    // set a limit to not flood the menu with entries
    Query::Query query( mainTerm );
    query.setLimit( s_maxActionsPerSection+1 );

    kDebug() << query.toSparqlQuery();

    // start the query
    Soprano::Util::AsyncQuery* sQuery
        = Soprano::Util::AsyncQuery::executeQuery( ResourceManager::instance()->mainModel(),
                                                   query.toSparqlQuery(),
                                                   Soprano::Query::QueryLanguageSparql );
    q->connect( sQuery, SIGNAL( nextReady( Soprano::Util::AsyncQuery* ) ),
                SLOT( _k_nextRelateToResourceReady( Soprano::Util::AsyncQuery* ) ) );
    q->connect( sQuery, SIGNAL( finished( Soprano::Util::AsyncQuery* ) ),
                SLOT( _k_relateToResourceQueryFinished( Soprano::Util::AsyncQuery* ) ) );

    // we do not want the query to return deprecated results after the menu has been deleted
    sQuery->setParent( q );
}


void Nepomuk::AnnotationMenu::Private::addContextActions( KMenu* menu )
{
    kDebug();

    m_contextActionMenu = menu;

    // add widget action showing a busy thingi and query the resources async
    m_contextQueryBusyAction = q->createBusyAction(menu);
    m_contextActionMenu->addAction( m_contextQueryBusyAction );

    // query resources of the supported types that have a high usage count
    Query::AndTerm mainTerm;

    // restrict to one of the supported resource types
    mainTerm.addSubTerm( createResourceTypeTerm() );

    // sort the resources by most often used
    Query::ComparisonTerm ct( Nepomuk::Vocabulary::NUAO::usageCount(), Query::Term() );
    ct.setSortWeight( 1, Qt::DescendingOrder );
    mainTerm.addSubTerm( Query::OptionalTerm::optionalizeTerm( ct ) );

    // set a limit to not flood the menu with entries
    Query::Query query( mainTerm );
    query.setLimit( s_maxActionsPerSection+1 );

    kDebug() << query.toSparqlQuery();

    // start the query
    Soprano::Util::AsyncQuery* sQuery
        = Soprano::Util::AsyncQuery::executeQuery( ResourceManager::instance()->mainModel(),
                                                   query.toSparqlQuery(),
                                                   Soprano::Query::QueryLanguageSparql );
    q->connect( sQuery, SIGNAL( nextReady( Soprano::Util::AsyncQuery* ) ),
                SLOT( _k_nextContextReady( Soprano::Util::AsyncQuery* ) ) );
    q->connect( sQuery, SIGNAL( finished( Soprano::Util::AsyncQuery* ) ),
                SLOT( _k_contextQueryFinished( Soprano::Util::AsyncQuery* ) ) );

    // we do not want the query to return deprecated results after the menu has been deleted
    sQuery->setParent( q );
}


void Nepomuk::AnnotationMenu::Private::addAnnotationActions( KMenu* menu )
{
    kDebug();
    if ( m_resources.count() == 1 ) {

        m_annotationActionCount = 0;

        // add widget action showing a busy thingi and query the resources async
        m_annotationsBusyAction = q->createBusyAction(menu);
        menu->addAction( m_annotationsBusyAction );

        m_annotationTitleCreated = false;

        if ( !m_annotationModel ) {
            m_annotationModel = new ResourceAnnotationModel( q );
            q->connect( m_annotationModel, SIGNAL( newAnnotation( Nepomuk::Annotation* ) ),
                        SLOT( _k_newAnnotation( Nepomuk::Annotation* ) ) );
            q->connect( m_annotationModel, SIGNAL( finished() ),
                        SLOT( _k_annotationModelFinished() ) );
        }

        m_annotationModel->setResource( m_resources.first() );
    }
}


void Nepomuk::AnnotationMenu::Private::addTagActions( KMenu* menu )
{
    kDebug();

    m_tagActionMenu = menu;

    m_tagActionCount = 0;

    // add widget action showing a busy thingi and query the resources async
    m_tagBusyAction = q->createBusyAction(menu);
    menu->addAction( m_tagBusyAction );

    // query most used tags
    Query::AndTerm mainTerm;

    // we only want tags
    mainTerm.addSubTerm( Query::ResourceTypeTerm( Soprano::Vocabulary::NAO::Tag() ) );

    // sort the tags by most often used
    Query::ComparisonTerm ct( Soprano::Vocabulary::NAO::hasTag(), Query::Term() );
    ct.setInverted( true );
    ct.setSortWeight( 1, Qt::DescendingOrder );
    mainTerm.addSubTerm( Query::OptionalTerm::optionalizeTerm( ct ) );

    // set a limit to not flood the menu with entries
    Query::Query query( mainTerm );
    query.setLimit( s_maxActionsPerSection+1 );

    kDebug() << query.toSparqlQuery();

    // start the query
    Soprano::Util::AsyncQuery* sQuery
        = Soprano::Util::AsyncQuery::executeQuery( ResourceManager::instance()->mainModel(),
                                                   query.toSparqlQuery(),
                                                   Soprano::Query::QueryLanguageSparql );
    q->connect( sQuery, SIGNAL( nextReady( Soprano::Util::AsyncQuery* ) ),
                SLOT( _k_nextTagReady( Soprano::Util::AsyncQuery* ) ) );
    q->connect( sQuery, SIGNAL( finished( Soprano::Util::AsyncQuery* ) ),
                SLOT( _k_tagQueryFinished( Soprano::Util::AsyncQuery* ) ) );

    // we do not want the query to return deprecated results after the menu has been deleted
    sQuery->setParent( q );
}


void Nepomuk::AnnotationMenu::Private::addSearchAction( KMenu* menu )
{
    kDebug();
    if ( !m_searchMenu ) {
        m_searchMenu = new SearchMenu( menu );
        m_searchMenu->setConfigurationFlags( SearchMenu::SearchWhileYouType );
        q->connect( m_searchMenu, SIGNAL( resultTriggered( Nepomuk::Query::Result ) ),
                    SLOT( _k_searchResultTriggered( Nepomuk::Query::Result ) ) );
    }
    QAction* action = menu->addMenu( m_searchMenu );
    action->setText( i18nc( "@action:inmenu", "Search..." ) );
}


void Nepomuk::AnnotationMenu::Private::addOpenWithAction( KMenu* menu )
{
    kDebug();
    if ( !KStandardDirs::findExe( QLatin1String( "ginkgo" ) ).isEmpty() ) {
        QAction* action = new QAction( menu );
        action->setText( i18nc( "@action:inmenu", "Open in Ginkgo" ) );
        action->setIcon( KIcon( "nepomuk" ) );
        q->connect( action, SIGNAL( triggered() ), SLOT( _k_openWithActionTriggered() ) );
        menu->addAction( action );
    }
}


namespace {
    QString resourceLabel( Nepomuk::Resource resource ) {
        QString label = resource.pimoThing().label();
        if ( label.isEmpty() )
            label = resource.genericLabel();
        return label;
    }
}

void Nepomuk::AnnotationMenu::Private::_k_menuAboutToShow()
{
    emit q->aboutToBeConstructed( q );

    if ( !m_menuCreated ) {
        m_menuCreated = true;

        KMenu* relateMenu = q;

        if ( !m_resources.isEmpty() ) {
            if ( m_resources.count() == 1 )
                q->addTitle( KIcon( "nepomuk" ), i18nc( "@title menu title", "Annotate %1", resourceLabel( m_resources.first() ) ) );
            else
                q->addTitle( KIcon( "nepomuk" ), i18nc( "@title menu title", "Annotate resource" ) );

            if ( m_configFlags & AnnotationMenu::ShowRatingAction ) {
                addRatingAction( q );
                if ( m_configFlags & AnnotationMenu::UseSubMenus ) {
                    q->addSeparator();
                }
            }

            if ( m_configFlags & AnnotationMenu::ShowTagActions ) {
                const QString title = i18nc( "@title:menu title above a set of tags that can be applied to the current resource.",
                                             "Tag with" );
                if ( m_configFlags & AnnotationMenu::UseSubMenus ) {
                    KMenu* subMenu = new KMenu( title, q );
                    q->addMenu( subMenu );
                    addTagActions( subMenu );
                }
                else {
                    q->addTitle( title + ':' );
                    addTagActions( q );
                }
            }

            if ( m_configFlags & ( AnnotationMenu::ShowRelatedResourcesActions|AnnotationMenu::ShowSearchAction|AnnotationMenu::ShowNewResourceActions ) ) {
                const QString relateToTitle = i18nc( "@title:menu title above a set of resources that can be marked as related to the current resource.",
                                                     "Relate to" );
                if ( m_configFlags & AnnotationMenu::UseSubMenus ) {
                    relateMenu = new KMenu( relateToTitle, q );
                    q->addMenu( relateMenu );
                }
                else {
                    q->addTitle( relateToTitle + ':' );
                }
            }
        }
        else {
            q->addTitle( i18nc( "@title:menu title above a set of resources that can be set as the current working context.",
                                "Work in Context of:" ) );
        }

        if ( m_configFlags & AnnotationMenu::ShowSearchAction ) {
            addSearchAction( relateMenu );
            if ( m_resources.isEmpty() )
                m_searchMenu->setCoreQueryTerm( createResourceTypeTerm() );
            else
                m_searchMenu->setCoreQueryTerm( Query::Term() );
            relateMenu->addSeparator();
        }

        if ( !m_resources.isEmpty() ) {
            if ( m_configFlags & AnnotationMenu::ShowRelatedResourcesActions ) {
                if ( m_configFlags & AnnotationMenu::UseSubMenus ) {
                    addRelateToActions( relateMenu );
                }
                else {
                    addRelateToActions( q );
                }
            }
        }
        else {
            if ( m_configFlags & AnnotationMenu::UseSubMenus ) {
                KMenu* subMenu = new KMenu( i18n( "Previously used" ), q );
                q->addMenu( subMenu );
                addContextActions( subMenu );
            }
            else {
                addContextActions( q );
            }
        }

        if ( m_configFlags & AnnotationMenu::ShowNewResourceActions ) {
            relateMenu->addSeparator();
            addNewResourceActions( relateMenu );
        }

        if ( !m_resources.isEmpty() ) {
            if ( m_configFlags & AnnotationMenu::ShowAnnotationActions ) {
                // a header will only be added in case that we actually find some possible annotations
                addAnnotationActions( q );
            }

            if ( m_configFlags & AnnotationMenu::ShowOpenWithAction ) {
                q->addSeparator();
                addOpenWithAction( q );
            }
        }

        emit q->aboutToShow( q, true );
    }
    else {
        emit q->aboutToShow( q, false );
    }
}


void Nepomuk::AnnotationMenu::Private::_k_ratingChanged( unsigned int rating )
{
    kDebug();
    for ( int i = 0; i < m_resources.count(); ++i )
        m_resources[i].setRating( rating );
    emit q->resourcesAnnotated();
}


void Nepomuk::AnnotationMenu::Private::_k_relateToActionTriggered()
{
    kDebug();
    Nepomuk::Resource res = qobject_cast<QAction*>( q->sender() )->data().value<Nepomuk::Resource>();
    for ( int i = 0; i < m_resources.count(); ++i )
        m_resources[i].addProperty( Nepomuk::Vocabulary::PIMO::isRelated(), res.pimoThing() );
    emit q->resourcesAnnotated();
}


void Nepomuk::AnnotationMenu::Private::_k_contextActionTriggered()
{
    kDebug();
    Nepomuk::Resource res = qobject_cast<QAction*>( q->sender() )->data().value<Nepomuk::Resource>();
    ContextServiceInterface().setCurrentContext( res );
}


void Nepomuk::AnnotationMenu::Private::_k_newResourceActionTriggered()
{
    kDebug();
    Nepomuk::Types::Class type = qobject_cast<QAction*>( q->sender() )->data().value<Nepomuk::Types::Class>();
    Nepomuk::Resource newRes = NewResourceDialog::createResource( type, q );
    if ( newRes.isValid() ) {
        if ( !m_resources.isEmpty() ) {
            for ( int i = 0; i < m_resources.count(); ++i )
                m_resources[i].addProperty( Nepomuk::Vocabulary::PIMO::isRelated(), newRes.pimoThing() );
            emit q->resourcesAnnotated();
        }
        else {
            ContextServiceInterface().setCurrentContext( newRes );
        }
    }
}


void Nepomuk::AnnotationMenu::Private::_k_annotationActionTriggered()
{
    kDebug();
    Nepomuk::Annotation* annotation = qobject_cast<QAction*>( q->sender() )->data().value<Nepomuk::Annotation*>();
    annotation->create( m_resources.first() );
    emit q->resourcesAnnotated();
}


void Nepomuk::AnnotationMenu::Private::_k_searchResultTriggered( const Nepomuk::Query::Result& result )
{
    kDebug();
    Nepomuk::Resource res = result.resource();
    if ( !m_resources.isEmpty() ) {
        for ( int i = 0; i < m_resources.count(); ++i )
            m_resources[i].addProperty( Nepomuk::Vocabulary::PIMO::isRelated(), res.pimoThing() );
        emit q->resourcesAnnotated();
    }
    else {
        ContextServiceInterface().setCurrentContext( res );
    }
}


void Nepomuk::AnnotationMenu::Private::_k_tagActionToggled( bool enabled )
{
    kDebug();
    Nepomuk::Tag tag = qobject_cast<QAction*>( q->sender() )->data().value<Nepomuk::Resource>();
    for ( int i = 0; i < m_resources.count(); ++i ) {
        if ( enabled )
            m_resources[i].addTag( tag );
        else
            m_resources[i].removeProperty( Soprano::Vocabulary::NAO::hasTag(), tag );
    }
    emit q->resourcesAnnotated();
}


void Nepomuk::AnnotationMenu::Private::_k_openWithActionTriggered()
{
    kDebug();
    QStringList uris;
    Q_FOREACH( const Resource& res, m_resources )
        uris << KUrl( res.resourceUri() ).url();
    QProcess::startDetached( KStandardDirs::findExe( QLatin1String( "ginkgo" ) ), uris );
    kDebug() << KStandardDirs::findExe( QLatin1String( "ginkgo" ) ) << uris;
}


void Nepomuk::AnnotationMenu::Private::_k_nextRelateToResourceReady( Soprano::Util::AsyncQuery* query )
{
    kDebug();
    // HACK: workaround for a Virtuoso bug where URIs are returned as strings if the query contains an aggregate function
    m_relateToActionMenu->insertAction( m_relateToBusyAction, createRelateToAction( QUrl( query->binding( 0 ).toString() ) ) );
    // TODO: if we have more than s_maxActionsPerSection results, ignore the last result and add a "more..." action
    query->next();
}


void Nepomuk::AnnotationMenu::Private::_k_relateToResourceQueryFinished( Soprano::Util::AsyncQuery* )
{
    kDebug();
    delete m_relateToBusyAction;
}


void Nepomuk::AnnotationMenu::Private::_k_nextContextReady( Soprano::Util::AsyncQuery* query )
{
    kDebug();
    // HACK: workaround for a Virtuoso bug where URIs are returned as strings if the query contains an aggregate function
    m_contextActionMenu->insertAction( m_contextQueryBusyAction, createContextAction( QUrl( query->binding( 0 ).toString() ) ) );
    // TODO: if we have more than s_maxActionsPerSection results, ignore the last result and add a "more..." action
    query->next();
}


void Nepomuk::AnnotationMenu::Private::_k_contextQueryFinished( Soprano::Util::AsyncQuery* )
{
    kDebug();
    delete m_contextQueryBusyAction;
}


void Nepomuk::AnnotationMenu::Private::_k_nextTagReady( Soprano::Util::AsyncQuery* query )
{
    // HACK: workaround for a Virtuoso bug where URIs are returned as strings if the query contains an aggregate function
    m_tagActionMenu->insertAction( m_tagBusyAction, createTagAction( QUrl( query->binding( 0 ).toString() ) ) );
    if ( ++m_tagActionCount > s_maxActionsPerSection ) {
        // add the "more..." action
        m_tagActionMenu->insertAction( m_tagBusyAction, createShowAllTagsAction() );

        // we do not need the query anymore
        query->close();
    }
    else {
        query->next();
    }
}


void Nepomuk::AnnotationMenu::Private::_k_tagQueryFinished( Soprano::Util::AsyncQuery* )
{
    kDebug();
    delete m_tagBusyAction;
}


void Nepomuk::AnnotationMenu::Private::_k_newAnnotation( Nepomuk::Annotation* annotation )
{
    kDebug();

    // FIXME: make sure we do not get duplicates
    if ( !annotation->exists( m_resources.first() ) ) {
        if ( ++m_annotationActionCount > s_maxActionsPerSection ) {
            // do it the stupid way
            // TODO: have a close method or something on the annotation model
            return;
        }

        if ( !( m_configFlags & AnnotationMenu::UseSubMenus ) &&
             !m_annotationTitleCreated ) {
            m_annotationTitleCreated = true;
            q->addTitle( i18nc( "@title:menu title above a set of possible annotations for the current resource.",
                                "Annotate:" ),
                         m_annotationsBusyAction );
        }

        q->insertAction( m_annotationsBusyAction, createAnnotationAction( annotation ) );
    }
}


void Nepomuk::AnnotationMenu::Private::_k_annotationModelFinished()
{
    kDebug();
    delete m_annotationsBusyAction;
}


namespace {
    // This is from TagWidget in kdelibs - would be great to somehow share this code
    QList<Nepomuk::Tag> intersectResourceTags( const QList<Nepomuk::Resource>& resources )
    {
        if ( resources.count() == 1 ) {
            return resources.first().tags();
        }
        else if ( !resources.isEmpty() ) {
            // determine the tags used for all resources
            QSet<Nepomuk::Tag> tags = QSet<Nepomuk::Tag>::fromList( resources.first().tags() );
            QList<Nepomuk::Resource>::const_iterator it = resources.constBegin();
            for ( ++it; it != resources.constEnd(); ++it ) {
                tags.intersect( QSet<Nepomuk::Tag>::fromList( (*it).tags() ) );
            }
            return tags.values();
        }
        else {
            return QList<Nepomuk::Tag>();
        }
    }
}

void Nepomuk::AnnotationMenu::Private::_k_showAllTags()
{
    kDebug();

    KEditTagsDialog dlg( intersectResourceTags( m_resources ), q );
    if( dlg.exec() ) {
        Nepomuk::MassUpdateJob* job = Nepomuk::MassUpdateJob::tagResources( m_resources, dlg.tags() );
        job->start();
        emit q->resourcesAnnotated();
    }
}


void Nepomuk::AnnotationMenu::Private::populateAction( QAction* action, const Nepomuk::Resource& res, bool withTypeLabel ) const
{
    q->setElidedActionText( action, res.genericLabel(), withTypeLabel ? Types::Class( res.resourceType() ).label() : QString() );
    QString tooltip = res.genericLabel();
    if ( withTypeLabel ) {
        tooltip += QLatin1String( " (" );
        tooltip += Types::Class( res.resourceType() ).label();
        tooltip += ')';
    }
    action->setToolTip( tooltip );
    QString iconName( res.genericIcon() );
    if ( iconName.isEmpty() ) {
        QIcon icon = Types::Class( res.resourceType() ).icon();
        if ( !icon.isNull() )
            action->setIcon( icon );
    }
    else {
        action->setIcon( KIcon( iconName ) );
    }
    action->setData( QVariant::fromValue( res ) );
}


Nepomuk::AnnotationMenu::AnnotationMenu( QWidget* parent )
    : Menu( parent ),
      d( new Private() )
{
    kDebug();
    d->q = this;
    connect( this, SIGNAL( aboutToShow() ), this, SLOT( _k_menuAboutToShow() ) );
}


Nepomuk::AnnotationMenu::~AnnotationMenu()
{
    kDebug();
    delete d;
}


void Nepomuk::AnnotationMenu::setConfigurationFlags( ConfigurationFlags flags )
{
    d->m_configFlags = flags;
    d->clear();
}


Nepomuk::AnnotationMenu::ConfigurationFlags Nepomuk::AnnotationMenu::configurationFlags() const
{
    return d->m_configFlags;
}


QList<Nepomuk::Types::Class> Nepomuk::AnnotationMenu::supportedTypes() const
{
    QList<Nepomuk::Types::Class> types;
    Q_FOREACH( const Private::TypeElement& elem, d->resourceTypes() ) {
        types << elem.first;
    }
    return types;
}


void Nepomuk::AnnotationMenu::addSupportedType( const Types::Class& type, const Query::Term& queryTerm )
{
    d->m_resourceTypes.append( qMakePair( type, queryTerm ) );
    d->clear();
}


void Nepomuk::AnnotationMenu::setSupportedTypes( const QList<Types::Class>& types )
{
#warning Add the task query
    d->m_resourceTypes.clear();
    Q_FOREACH( const Types::Class& type, types ) {
        d->m_resourceTypes.append( qMakePair( type, Query::Term() ) );
    }
    d->clear();
}


void Nepomuk::AnnotationMenu::setResource( const Resource& res )
{
    setResources( QList<Resource>() << res );
}


void Nepomuk::AnnotationMenu::setResources( const QList<Resource>& res )
{
    d->m_resources = res;
    d->clear();
}

#include "annotationmenu.moc"
