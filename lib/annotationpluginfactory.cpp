/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
 * Copyright (C) 2008-2009 Sebastian Trueg <trueg@kde.org>
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

#include <QtCore/QVariant>

#include <Nepomuk/Types/Property>
#include <Nepomuk/Types/Class>
#include <Nepomuk/Resource>

#include <kservicetypetrader.h>
#include <kservice.h>
#include <kdebug.h>

#include "annotationpluginfactory.h"
#include "annotationplugin.h"


class Nepomuk::AnnotationPluginFactory::Private
{
public:
};



Nepomuk::AnnotationPluginFactory::AnnotationPluginFactory(QObject* parent)
    : QObject(parent),
      d( new Private )
{
}


Nepomuk::AnnotationPluginFactory::~AnnotationPluginFactory()
{
    delete d;
}


K_GLOBAL_STATIC( Nepomuk::AnnotationPluginFactory, s_globalFactory )

Nepomuk::AnnotationPluginFactory* Nepomuk::AnnotationPluginFactory::instance()
{
    return s_globalFactory;
}


QList<Nepomuk::AnnotationPlugin*> Nepomuk::AnnotationPluginFactory::getPluginsSupportingAnnotationOfTypes( const QList<Types::Class>& rdfTypes )
{
	QList<AnnotationPlugin*> plugins;

    QStringList subQueries( "'*' in [X-Nepomuk-ResourceTypes]" );
    for( int i = 0; i < rdfTypes.count(); ++i ) {
        subQueries << QString( "'%1' in [X-Nepomuk-ResourceTypes]" ).arg( rdfTypes[i].uri().toString() );
    }

	KService::List services
        = KServiceTypeTrader::self()->query( "Nepomuk/AnnotationPlugin", subQueries.join( " or " ) );

	foreach(const KSharedPtr<KService>& service, services) {
		// Return the cached instance if we have one, create a new one else
        AnnotationPlugin* plugin = getPluginInstance(service);
        if ( plugin && !plugins.contains( plugin ) )
            plugins.append( plugin );
    }

	return plugins;
}


QList<Nepomuk::AnnotationPlugin*> Nepomuk::AnnotationPluginFactory::getPluginsSupportingAnnotationOfType( const Types::Class& rdfType )
{
    return getPluginsSupportingAnnotationOfTypes( QList<Types::Class>() << rdfType );
}


QList<Nepomuk::AnnotationPlugin*> Nepomuk::AnnotationPluginFactory::getPluginsSupportingAnnotationForResource( const QUrl& resource )
{
    Nepomuk::Resource res( resource );
    QList<Types::Class> types;
    foreach( const QUrl& type, res.types() ) {
        Types::Class typeClass( type );
        types << typeClass;
        types << typeClass.allParentClasses();
    }
    return getPluginsSupportingAnnotationOfTypes( types );
}


QList<Nepomuk::AnnotationPlugin*> Nepomuk::AnnotationPluginFactory::getAllPlugins()
{
	QList<AnnotationPlugin*> plugins;

	KService::List services = KServiceTypeTrader::self()->query("Nepomuk/AnnotationPlugin");

	foreach(const KSharedPtr<KService>& service, services) {
		// Return the cached instance if we have one, create a new one else
        AnnotationPlugin* plugin = getPluginInstance(service);
        if ( plugin && !plugins.contains( plugin ) )
            plugins.append( plugin );
    }

	return plugins;
}

Nepomuk::AnnotationPlugin* Nepomuk::AnnotationPluginFactory::getPluginInstance(const KSharedPtr<KService>& service)
{
    return service->createInstance<AnnotationPlugin>();
}

#include "annotationpluginfactory.moc"

