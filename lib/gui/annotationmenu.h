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

#ifndef _NEPOMUK_ANNOTATION_MENU_H_
#define _NEPOMUK_ANNOTATION_MENU_H_

#include "nepomukmenu.h"

#include <Nepomuk/Query/Term>

#include "nepomukannotation_export.h"

class QPoint;

namespace Soprano {
    namespace Util {
        class AsyncQuery;
    }
}

// TODO: add protected method or some other way to get confirmation before actually creating an annotation
//       the simplest would be: "virtual bool confirmAnnotation( const QString& humanReadableDescription ) const"

namespace Nepomuk {

    class Resource;
    namespace Types {
        class Class;
    }

    /**
     * \class AnnotationMenu annotationmenu.h
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class NEPOMUKANNOTATION_EXPORT AnnotationMenu : public Menu
    {
        Q_OBJECT

    public:
        /**
         * Create a new menu.
         */
        AnnotationMenu( QWidget* parent = 0 );

        /**
         * Destructor
         */
        virtual ~AnnotationMenu();

        enum ConfigurationFlag {
            NoFlags = 0x0,
            ShowRatingAction = 0x1,
            ShowTagActions = 0x2,
            ShowRelatedResourcesActions = 0x4,
            ShowAnnotationActions = 0x8,
            ShowNewResourceActions = 0x10,
            ShowSearchAction = 0x20,
            ShowOpenWithAction = 0x40,
            ShowAll = ShowRatingAction|ShowTagActions|ShowRelatedResourcesActions|ShowAnnotationActions|ShowNewResourceActions|ShowSearchAction|ShowOpenWithAction,
            UseSubMenus = 0x40
        };
        Q_DECLARE_FLAGS( ConfigurationFlags, ConfigurationFlag )

        void setConfigurationFlags( ConfigurationFlags flags );
        ConfigurationFlags configurationFlags() const;

        QList<Types::Class> supportedTypes() const;

        /**
         * The supported resource types can be changed with this method. By default
         * PIMO:Task, PIMO:Project, and PIMO:Person are used.
         *
         * \param type The type to support
         * \param queryTerm The query term which is to be used to query resources of that type. In most
         * cases the default (just match the type) is sufficient. But there are special cases where
         * more is necessary. A typical example are tasks which can be finished or not and this fact
         * should be handled in \p queryTerm.
         */
        void addSupportedType( const Types::Class& type, const Query::Term& queryTerm = Query::Term() );

        /**
         * Set a list of supported types. Possible detailed queries as can be specified in addSupportedType()
         * are created automatically. For now this is only done for pimo:Task.
         */
        void setSupportedTypes( const QList<Types::Class>& types );

        /**
         * The same as calling setResources() with a list containg \p res.
         */
        void setResource( const Resource& res );

        /**
         * Set the resources that should be annotated by the menu. If the list is empty
         * actions to change the context or create new resources are created.
         */
        void setResources( const QList<Resource>& res );

    Q_SIGNALS:
        /**
         * This signal is emitted whenever one of the actions is actually executed.
         * This can be useful when annotating new resources that need some post-processing
         * only in case they are actually created.
         */
        void resourcesAnnotated();

        /**
         * %AnnotationMenu is a dynamic menu which is build based on its settings.
         * In certain situations these settings are not clear until the very moment
         * the menu will be constructed. This signal can be used to configure the menu
         * before it will be shown.
         */
        void aboutToBeConstructed( Nepomuk::AnnotationMenu* menu );

        /**
         * Since AnnotationMenu uses the QMenu::aboutToShow() signal internally
         * and connection order is not guranteed in Qt it is recommended to use
         * this signal instead to ensure that added actions are actually inserted
         * in the wanted position and are not removed by AnnotationMenu after
         * adding them.
         *
         * \param menu A convinience pointer to the AnnotationMenu
         * \param menuRecreated \p true if the contents of the menu where newly created
         * for this showing of the menu. This also means that any actions added before
         * have been removed.
         */
        void aboutToShow( Nepomuk::AnnotationMenu* menu, bool menuRecreated = false );

    private:
        class Private;
        Private* const d;

        Q_PRIVATE_SLOT( d, void _k_menuAboutToShow() )
        Q_PRIVATE_SLOT( d, void _k_ratingChanged(unsigned int) )
        Q_PRIVATE_SLOT( d, void _k_relateToActionTriggered() )
        Q_PRIVATE_SLOT( d, void _k_contextActionTriggered() )
        Q_PRIVATE_SLOT( d, void _k_newResourceActionTriggered() )
        Q_PRIVATE_SLOT( d, void _k_annotationActionTriggered() )
        Q_PRIVATE_SLOT( d, void _k_searchResultTriggered( const Nepomuk::Query::Result& ) )
        Q_PRIVATE_SLOT( d, void _k_tagActionToggled(bool) )
        Q_PRIVATE_SLOT( d, void _k_openWithActionTriggered() )
        Q_PRIVATE_SLOT( d, void _k_nextRelateToResourceReady( Soprano::Util::AsyncQuery* ) )
        Q_PRIVATE_SLOT( d, void _k_relateToResourceQueryFinished( Soprano::Util::AsyncQuery* ) )
        Q_PRIVATE_SLOT( d, void _k_nextContextReady( Soprano::Util::AsyncQuery* ) )
        Q_PRIVATE_SLOT( d, void _k_contextQueryFinished( Soprano::Util::AsyncQuery* ) )
        Q_PRIVATE_SLOT( d, void _k_nextTagReady( Soprano::Util::AsyncQuery* ) )
        Q_PRIVATE_SLOT( d, void _k_tagQueryFinished( Soprano::Util::AsyncQuery* ) )
        Q_PRIVATE_SLOT( d, void _k_newAnnotation( Nepomuk::Annotation* ) )
        Q_PRIVATE_SLOT( d, void _k_annotationModelFinished() )
        Q_PRIVATE_SLOT( d, void _k_showAllTags() )
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS( Nepomuk::AnnotationMenu::ConfigurationFlags )

#endif
