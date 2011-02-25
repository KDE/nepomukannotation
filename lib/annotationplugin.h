/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
 * Copyright (c) 2008-2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef _NEPOMUK_ANNOTATION_PLUGIN_H_
#define _NEPOMUK_ANNOTATION_PLUGIN_H_

#include <QtCore/QObject>

#include "nepomukannotation_export.h"

#include <kpluginfactory.h>
#include <kpluginloader.h>

namespace Soprano {
	class Node;
}

namespace Nepomuk {

    class Annotation;
    class AnnotationRequest;

    /**
     * \class AnnotationPlugin annotationplugin.h AnnotationPlugin
     *
     * \brief A generic implementation of a Nepomuk AnnotationPlugin
     *
     * All of the work is done via the doGetPossibleAnnotations method.
     *
     * \author Adrien Bustany <madcat@mymadcat.com>, Sebastian Trueg <trueg@kde.org>
     */
    class NEPOMUKANNOTATION_EXPORT AnnotationPlugin : public QObject
    {
        Q_OBJECT

    public:
        AnnotationPlugin( QObject* parent );
        virtual ~AnnotationPlugin();

    Q_SIGNALS:
        /**
         * Emitted when addRelation finishes
         */
        void finished();

/** \cond hide_private_signals_from_moc */
#ifndef Q_MOC_RUN
    private: // don't tell moc, but this signal is in fact private
#endif
/** \endcond */
        /**
         * Emitted once the plugin has listed all possible annotations.
         *
         * This is a private signal, it can't be emitted directly
         * by subclasses of AnnotationPlugin, use emitFinished() instead.
         *
         * \param plugin The plugin emitting the signal.
         */
        void finished( Nepomuk::AnnotationPlugin* plugin );

        /**
         * Emitted once a new possible Annotation has been found.
         * Until finished has been emitted, more annotations can still arrive.
         *
         * This is a private signal, it can't be emitted directly
         * by subclasses of AnnotationPlugin, use addNewAnnotation() instead.
         *
         * \param annotation The new Annotation object. While the object is destroyed
         * with the plugin, it is recommended to do the destruction manually once
         * the Annotation object is no longer needed.
         */
        void newAnnotation( Nepomuk::Annotation* annotation );

    public Q_SLOTS:
        /**
         * Searches for possible annotations of resource that match filter.
         *
         * Annotations are listed via newAnnotation. Once all have been listed,
         * finished is emitted.
         *
         * \param request The annotation request describing the resource and context
         * of the possible annotations.
         */
        void getPossibleAnnotations( const AnnotationRequest& request );

    protected:
        /**
         * Subclasses should reimplement this to provide a list of Annotation objects.
         * Be aware that the method may be called again before the finished signal is
         * emitted. In that case the plugin should restart looking for annotations
         * without emitting the finished signal.
         *
         * \param request The annotation request describing the resource and context
         * of the possible annotations. See its documentation for details.
         */
        virtual void doGetPossibleAnnotations( const AnnotationRequest& request ) = 0;

        /**
         * Get the current request as passed to getPossibleAnnotations. Thus, it is not
         * necessary to store the request in implementations.
         */
        AnnotationRequest currentRequest() const;

    protected Q_SLOTS:
        /**
         * Emits the finished signal.
         */
        void emitFinished();

        /**
         * Subclasses need to call this for each new Annotation as a result of
         * getPossibleAnnotations.
         *
         * This will emit the newAnnotation signal.
         */
        void addNewAnnotation( Nepomuk::Annotation* );

    private:
        class Private;
        Private* const d;

        friend class Annotation;
    };

} // End namespace Nepomuk


/**
 * Export a %Nepomuk Annotation Plugin.
 *
 * \param classname The name of the Nepomuk::AnnotationPlugin subclass to export.
 * \param libname The name of the library which should export the plugin.
 */
#define NEPOMUK_EXPORT_ANNOTATION_PLUGIN( classname, libname )  \
K_PLUGIN_FACTORY(factory, registerPlugin<classname>();)     \
K_EXPORT_PLUGIN(factory(#libname))

#endif
