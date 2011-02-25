/*
   Copyright (c) 2008-2009 Sebastian Trueg <trueg@kde.org>

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

#ifndef _NEPOMUK_ANNOTATION_PLUGIN_WRAPPER_H_
#define _NEPOMUK_ANNOTATION_PLUGIN_WRAPPER_H_

#include <QtCore/QObject>
#include <QtCore/QList>

#include "nepomukannotation_export.h"
#include <nepomuk/resource.h>

#include "annotationplugin.h"


namespace Nepomuk {
    /**
     * \class AnnotationPluginWrapper annotationpluginwrapper.h annotationpluginwrapper.h
     *
     * \brief Simple wrapper around a set of AnnotationPlugins
     *
     * Typically one uses a set of AnnotationPlugins to create Annotation instances.
     * This simple class provides a wrapper to handle them as one.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class NEPOMUKANNOTATION_EXPORT AnnotationPluginWrapper : public QObject
    {
        Q_OBJECT

    public:
        AnnotationPluginWrapper( QObject* parent );
        ~AnnotationPluginWrapper();

        /**
         * Set the plugins to use for generating annotations. The wrapper
         * will take ownership of the plugins.
         */
        void setPlugins( const QList<AnnotationPlugin*>& plugins );

    public Q_SLOTS:
        /**
         * \sa AnnotationPlugin::getPossibleAnnotations
         */
        void getPossibleAnnotations( const Nepomuk::AnnotationRequest& request );

    Q_SIGNALS:
        /**
         * Emitted once all plugins finished.
         *
         * \sa AnnotationPlugin::finished
         */
        void finished();

        /**
         * \sa AnnotationPlugin::newAnnotation
         */
        void newAnnotation( Nepomuk::Annotation* annotation );

    private:
        class Private;
        Private* const d;

        Q_PRIVATE_SLOT( d, void _k_slotNewAnnotation( Nepomuk::Annotation* ) )
        Q_PRIVATE_SLOT( d, void _k_slotPluginFinished( Nepomuk::AnnotationPlugin* ) )
    };
}

#endif
