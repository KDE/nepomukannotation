/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ANNOTATIONPLUGINFACTORY_H
#define ANNOTATIONPLUGINFACTORY_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QUrl>

#include "nepomukannotation_export.h"

#include <ksharedptr.h>

class KService;

namespace Nepomuk {

    class AnnotationPlugin;

    namespace Types {
        class Property;
        class Class;
    }

    /**
     * \class AnnotationPluginFactory annotationpluginfactory.h AnnotationPluginFactory
     *
     * \brief Factory to create AnnotationPlugins based on the property you want to use. It can also return all known AnnotationPlugins.
     *
     * \author Adrien Bustany <madcat@mymadcat.com>, Sebastian Trueg <trueg@kde.org>
     */
    class NEPOMUKANNOTATION_EXPORT AnnotationPluginFactory : public QObject
    {
        Q_OBJECT

    public:
        /**
         * Create a new instance of AnnotationPluginFactory. It will
         * maintain a list of annotation plugins.
         */
        AnnotationPluginFactory(QObject* parent = 0);
        ~AnnotationPluginFactory();

        /**
         * \deprecated Create your own instance
         */
        static AnnotationPluginFactory* instance();

        /**
         * Returns a list with all the AnnotationPlugins than can handle the given \a property.
         *
         * The caller takes ownership of the returned plugins.
         */
        QList<AnnotationPlugin*> getPluginsForProperty(const Nepomuk::Types::Property& property);

        /**
         * Returns a list with all the AnnotationPlugins that return the given \a type
         *
         * The caller takes ownership of the returned plugins.
         */
        QList<AnnotationPlugin*> getPluginsForType(const QUrl& rdfType);

        /**
         * \return a list of plugins that can create Annotation objects for resource of type \p rdfType
         *
         * The caller takes ownership of the returned plugins.
         */
        QList<Nepomuk::AnnotationPlugin*> getPluginsSupportingAnnotationOfType( const Types::Class& rdfType );

        /**
         * \return a list of plugins that can create Annotation objects for resource of any type in \p rdfTypes
         *
         * The caller takes ownership of the returned plugins.
         */
        QList<Nepomuk::AnnotationPlugin*> getPluginsSupportingAnnotationOfTypes( const QList<Types::Class>& rdfTypes );

        /**
         * \return a list of plugins that can create Annotation objects for \p resource
         *
         * The caller takes ownership of the returned plugins.
         */
        QList<Nepomuk::AnnotationPlugin*> getPluginsSupportingAnnotationForResource( const QUrl& resource );

        /**
         * Tries to get the plugin with the given library \p name (usually something like nepomuk_xxxanotationplugin).
         * Can return NULL if ne plugin is found.
         *
         * The caller takes ownership of the returned plugins.
         */
        AnnotationPlugin* getPluginByLibraryName(const QString& name);

        /**
         * Returns a list with all the known AnnotationPlugins.
         *
         * The caller takes ownership of the returned plugins.
         */
        QList<AnnotationPlugin*> getAllPlugins();

    private:
        class Private;
        Private* const d;

        AnnotationPlugin* getPluginInstance(const KSharedPtr<KService>& service);
    };

} // End namespace Nepomuk

#endif
