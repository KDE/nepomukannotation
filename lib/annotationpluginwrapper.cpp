/*
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

#include "annotationpluginwrapper.h"
#include "annotationplugin.h"
#include "annotation.h"

#include "kdebug.h"

class Nepomuk::AnnotationPluginWrapper::Private
{
public:
    QList<AnnotationPlugin*> plugins;
    QList<AnnotationPlugin*> runningPlugins;

    void _k_slotNewAnnotation( Nepomuk::Annotation* );
    void _k_slotPluginFinished( Nepomuk::AnnotationPlugin* );

    AnnotationPluginWrapper* q;
};


void Nepomuk::AnnotationPluginWrapper::Private::_k_slotNewAnnotation( Nepomuk::Annotation* anno )
{
    kDebug() << anno->label() << anno->relevance();
    emit q->newAnnotation( anno );
}


void Nepomuk::AnnotationPluginWrapper::Private::_k_slotPluginFinished( Nepomuk::AnnotationPlugin* plugin )
{
    runningPlugins.removeAll( plugin );
    if ( runningPlugins.isEmpty() ) {
        emit q->finished();
    }
}



Nepomuk::AnnotationPluginWrapper::AnnotationPluginWrapper( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
    d->q = this;
}


Nepomuk::AnnotationPluginWrapper::~AnnotationPluginWrapper()
{
    delete d;
}


void Nepomuk::AnnotationPluginWrapper::setPlugins( const QList<AnnotationPlugin*>& plugins )
{
    qDeleteAll( d->plugins );

    d->plugins = plugins;

    foreach( Nepomuk::AnnotationPlugin* plugin, d->plugins ) {
        connect( plugin, SIGNAL( newAnnotation( Nepomuk::Annotation* ) ),
                 this, SLOT( _k_slotNewAnnotation( Nepomuk::Annotation* ) ) );
        connect( plugin, SIGNAL( finished( Nepomuk::AnnotationPlugin* ) ),
                 this, SLOT( _k_slotPluginFinished( Nepomuk::AnnotationPlugin* ) ) );
    }
}


void Nepomuk::AnnotationPluginWrapper::getPossibleAnnotations( const Nepomuk::AnnotationRequest& request )
{
    d->runningPlugins = d->plugins;
    foreach( Nepomuk::AnnotationPlugin* plugin, d->plugins ) {
        plugin->getPossibleAnnotations( request );
    }
}

#include "annotationpluginwrapper.moc"
