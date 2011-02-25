/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (c) 2009-2011 Sebastian Trueg <trueg@kde.org>
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

#ifndef _NEPOMUK_ANNOTATION_REQUEST_H_
#define _NEPOMUK_ANNOTATION_REQUEST_H_

#include <QtCore/QSharedDataPointer>
#include <QtCore/QList>
#include <QtCore/QString>

#include <nepomuk/resource.h>

#include "nepomukannotation_export.h"


// FIXME: annotations plugins can create annotations for specific types of resources as
//        stated in their desktop files. In case of text this is a problem as there is no
//        resource.
namespace Nepomuk {
    class AnnotationRequestPrivate;

    /**
     * \brief A container storing resource and filter/text for an annotation request.
     *
     * An annotation request has two parameters all of which are optional:
     *
     * \li A resource - The resource for which the annotation should be created. If it is not given
     *     annotations independant of a specific resource are to be created.
     * \li A filter - A filter is supposed to be a short string entered by the user which trigers
     *     an autocompletion via annotations. Plugins should also take the resource into account if
     *     it is set.
     *
     * If a plugin can not handle all of the information above it can simply choose to ignore them or
     * do nothing. However, it is recommended to always make sure that created annotations make sense.
     *
     * Be aware that already existing annotations can be filtered out via Annotation::exists and the
     * AnnotationRelevanceFilterModel.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class NEPOMUKANNOTATION_EXPORT AnnotationRequest
    {
    public:
        AnnotationRequest( const Resource& resource = Resource() );
        AnnotationRequest( const AnnotationRequest& );
        ~AnnotationRequest();

        AnnotationRequest& operator=( const AnnotationRequest& );

        Resource resource() const;
        QString filter() const;

        void setResource( const Resource& res );
        void setFilter( const QString& filter );

        static AnnotationRequest createSimpleFilterRequest( const Nepomuk::Resource& resource, const QString& filter );

    private:
        QSharedDataPointer<AnnotationRequestPrivate> d;
    };
}

#endif
