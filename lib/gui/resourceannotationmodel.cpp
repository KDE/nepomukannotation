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

#include "resourceannotationmodel.h"
#include "annotation.h"
#include "annotationpluginwrapper.h"
#include "annotationplugin.h"
#include "annotationpluginfactory.h"
#include "annotationrequest.h"
#include "nfo.h"
#include "nie.h"
#include "pimo.h"

#include <kicon.h>
#include <kdebug.h>
#include <kurl.h>

#include <Nepomuk/Resource>
#include <Nepomuk/Variant>

#include <QtCore/QFile>


class Nepomuk::ResourceAnnotationModel::Private
{
public:
    Private( ResourceAnnotationModel* parent )
        : m_autoUpdate( true ),
          q( parent ) {
    }

    AnnotationPluginWrapper* m_wrapper;
    AnnotationPluginFactory* pluginFactory;

    Nepomuk::Resource resource;
    QString filter;

    bool m_autoUpdate;

    void updateAnnotations();

private:
    ResourceAnnotationModel* q;
};


void Nepomuk::ResourceAnnotationModel::Private::updateAnnotations()
{
    AnnotationRequest request( resource );
    request.setFilter( filter );
    m_wrapper->getPossibleAnnotations( request );
}


Nepomuk::ResourceAnnotationModel::ResourceAnnotationModel( QObject* parent )
    : AnnotationModel( parent ),
      d( new Private(this) )
{
    d->pluginFactory = new AnnotationPluginFactory( this );
    d->m_wrapper = new AnnotationPluginWrapper( this );
    connect( d->m_wrapper, SIGNAL( newAnnotation( Nepomuk::Annotation* ) ),
             this, SLOT( addNewAnnotation( Nepomuk::Annotation* ) ) );
    connect( d->m_wrapper, SIGNAL( newAnnotation( Nepomuk::Annotation* ) ),
             this, SIGNAL( newAnnotation( Nepomuk::Annotation* ) ) );
    connect( d->m_wrapper, SIGNAL( finished() ),
             this, SIGNAL( finished() ) );
}


Nepomuk::ResourceAnnotationModel::~ResourceAnnotationModel()
{
    delete d;
}


Nepomuk::Resource Nepomuk::ResourceAnnotationModel::resource() const
{
    return d->resource;
}


void Nepomuk::ResourceAnnotationModel::setResource( const Nepomuk::Resource& res )
{
    d->resource = res;
    if ( d->m_autoUpdate )
        updateAnnotations();
}


void Nepomuk::ResourceAnnotationModel::setFilter( const QString& filter )
{
    d->filter = filter;
    if ( d->m_autoUpdate )
        updateAnnotations();
}


void Nepomuk::ResourceAnnotationModel::updateAnnotations()
{
    clear( true );
    doUpdateAnnotations();
}


void Nepomuk::ResourceAnnotationModel::doUpdateAnnotations()
{
    d->m_wrapper->setPlugins( AnnotationPluginFactory::instance()->getPluginsSupportingAnnotationForResource( resource().resourceUri() ) );
    d->updateAnnotations();
}


void Nepomuk::ResourceAnnotationModel::setAutoUpdate( bool b )
{
    d->m_autoUpdate = b;
}

#include "resourceannotationmodel.moc"
