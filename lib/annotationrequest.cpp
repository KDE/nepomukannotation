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

#include "annotationrequest.h"
#include "annotationrequest_p.h"

Nepomuk::AnnotationRequest::AnnotationRequest( const Resource& resource )
    : d( new AnnotationRequestPrivate() )
{
    d->resource = resource;
}


Nepomuk::AnnotationRequest::AnnotationRequest( const AnnotationRequest& other )
{
    d = other.d;
}


Nepomuk::AnnotationRequest::~AnnotationRequest()
{
}


Nepomuk::AnnotationRequest& Nepomuk::AnnotationRequest::operator=( const AnnotationRequest& other )
{
    d = other.d;
    return *this;
}


Nepomuk::Resource Nepomuk::AnnotationRequest::resource() const
{
    return d->resource;
}


QString Nepomuk::AnnotationRequest::filter() const
{
    return d->filter;
}


void Nepomuk::AnnotationRequest::setResource( const Resource& res )
{
    d->resource = res;
}


void Nepomuk::AnnotationRequest::setFilter( const QString& filter )
{
    d->filter = filter;
}


// static
Nepomuk::AnnotationRequest Nepomuk::AnnotationRequest::createSimpleFilterRequest( const Nepomuk::Resource& resource, const QString& filter )
{
    AnnotationRequest request( resource );
    request.setFilter( filter );
    return request;
}
