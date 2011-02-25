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

#ifndef _NEPOMUK_RESOURCE_ANNOTATION_MODEL_H_
#define _NEPOMUK_RESOURCE_ANNOTATION_MODEL_H_

#include "annotationmodel.h"

#include "nepomukannotation_export.h"

#include <Nepomuk/Resource>

namespace Nepomuk {

    class Annotation;

    /**
     * The ResourceAnnotationModel uses AnnotationPluginWrapper to get annotations
     * for a specific resource. If no content is set and none can be read from the
     * resource itself and the resource represents an existing file, Strigi is used
     * to extract the content.
     *
     * For pimo things their grounding occurrence's plain text is extracted.
     */
    class NEPOMUKANNOTATION_EXPORT ResourceAnnotationModel : public AnnotationModel
    {
        Q_OBJECT

    public:
        ResourceAnnotationModel( QObject* );
        virtual ~ResourceAnnotationModel();

        Resource resource() const;

    Q_SIGNALS:
        /**
         * Emitted once all annotations have been found
         */
        void finished();

        /**
         * \sa AnnotationPlugin::newAnnotation
         */
        void newAnnotation( Nepomuk::Annotation* annotation );

    public Q_SLOTS:
        void setResource( const Nepomuk::Resource& );
        void setFilter( const QString& );
        void updateAnnotations();

        /**
         * If enabled annotations are updated whenever
         * the resource, the content or the filter changes.
         *
         * Otherwise updateAnnotations() has to be called
         * manually.
         *
         * auto update is enabled by default.
         */
        void setAutoUpdate( bool b );

    protected:
        virtual void doUpdateAnnotations();

    private:
        class Private;
        Private* const d;
    };
}

#endif
