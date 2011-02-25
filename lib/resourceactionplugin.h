/* This file is part of the Nepomuk Project
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

#ifndef _NEPOMUK_RESOURCE_ACTION_PLUGIN_H_
#define _NEPOMUK_RESOURCE_ACTION_PLUGIN_H_

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include "nepomukannotation_export.h"

// THIS CLASS IS AN IDEA TO REPLACE ANNOTATIONPLUGIN WITH SOMETHING MORE GENERIC

// IDEA: the factory class could also act as a wrapper to get all actions for one resource, so there
//       is no need to check all plugins
// IDEA: ResoruceAction::execute should take a resource as parameter to be reusable

namespace Nepomuk {

    class ResourceAction;

    /**
     * \class ResourceActionPlugin resourceactionplugin.h Nepomuk/ResourceActionPlugin
     *
     * \brief Base class for all resource action plugins.
     *
     * Resource action plugins create ResourceAction objects for a specific resource
     * and an optional filter.
     *
     * FIXME: howto explaining the creation of new plugins + hints to scripting and simple actions.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class NEPOMUKANNOTATION_EXPORT ResourceActionPlugin : public QObject
    {
        Q_OBJECT

    public:
        /**
         * Constructor
         */
        ResourceActionPlugin( QObject* parent );

        /**
         * Destructor
         */
        virtual ~ResourceActionPlugin();
        
        /**
         * There are different type os actions that can be performed
         * on a resource.
         */
        enum ActionType {
            /**
             * An annotation action will add an annotation to the
             * resource. This can be a new type, a tag, or some other
             * relation to some other resource. It could even be a whole
             * set of new relations.
             */
            AnnotationAction = 1,

            /**
             * An open action will not change the resource but open it
             * in a specific application or simply display it in a fancy
             * way. Typical examples include opening a person in the addressbook
             * or writing an email to a person.
             */
            OpenAction = 2,

            AllActions = AnnotationAction|OpenAction
        };
        Q_DECLARE_FLAGS(ActionTypes, ActionType)

    public Q_SLOTS:
        /**
         * This triggers the creation of new actions in the plugin. New actions will be reported via
         * newAction. Once all actions have been reported, the finished signal is emitted.
         *
         * \param types The types of action that should be created.
         * \param resource The resource to be annotated
         * \param filter An optional string filter (most likely input from the user)
         *
         * \sa doGetPossibleActions
         */
        void getPossibleActions( const QUrl& resource, ActionTypes type = AllActions, const QString& filter = QString() );

    Q_SIGNALS:
        /**
         * Emitted once a new possible ResourceAction has been found.
         * Until finished has been emitted, more annotations can still arrive.
         *
         * \param action The new ResourceAction object. While the object is destroyed
         * with the plugin, it is recommended to do the destruction manually once
         * the ResourceAction object is no longer needed.
         *
         * \sa finished
         */        
        void newAction( Nepomuk::ResourceAction* action );

        /**
         * Emitted once all possible relations have been reported.
         */
        void finished();

    protected:
        /**
         * Subclasses should reimplement this to provide a list of Action objects.
         * Be aware that the method may be called again before the finished signal is
         * emitted.
         *
         * \param types The types of action that should be created.
         * \param resource The resource to be annotated
         * \param filter An optional string filter (most likely input from the user)s
         */
        virtual void doGetPossibleActions( const QUrl& resource, ActionTypes types, const QString& filter );

    private:
        class Private;
        Private* const d;

        Q_PRIVATE_SLOT( d, void slotGetPossibleActions() )
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Nepomuk::ResourceActionPlugin::ActionTypes)

/**
 * Export a %Nepomuk ResourceActionPlugin.
 *
 * \param classname The name of the Nepomuk::ResourceActionPlugin subclass to export.
 * \param libname The name of the library which should export the plugin.
 */
#define NEPOMUKANNOTATION_EXPORT_RESOURCE_ACTION_PLUGIN( classname, libname ) \
K_PLUGIN_FACTORY(factory, registerPlugin<classname>();) \
K_EXPORT_PLUGIN(factory(#libname))

#endif
