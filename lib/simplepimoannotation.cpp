/* This file is part of the Nepomuk Project
   Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>

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

#include "simplepimoannotation.h"
#include "pimomodel.h"

#include <nepomuk/variant.h>
#include <nepomuk/property.h>
#include <nepomuk/resourcemanager.h>
#include <nepomuk/thing.h>

#include <kicon.h>
#include <kdebug.h>



class Nepomuk::SimplePimoAnnotation::Private
{
public:
    QString newPropertyLabel;
};


Nepomuk::SimplePimoAnnotation::SimplePimoAnnotation( QObject* parent )
    : SimpleAnnotation( parent ),
      d( new Private() )
{
}


Nepomuk::SimplePimoAnnotation::SimplePimoAnnotation( const Types::Property& prop, const Variant& object, QObject* parent )
    : SimpleAnnotation( prop, object, parent ),
      d( new Private() )
{
}


Nepomuk::SimplePimoAnnotation::~SimplePimoAnnotation()
{
    delete d;
}


void Nepomuk::SimplePimoAnnotation::setNewPropertyLabel( const QString& s )
{
    d->newPropertyLabel = s;
    setIcon( KIcon( "document-new" ) );
}


QString Nepomuk::SimplePimoAnnotation::newPropertyLabel() const
{
    return d->newPropertyLabel;
}


bool Nepomuk::SimplePimoAnnotation::equals( Annotation* other ) const
{
    if ( SimplePimoAnnotation* sa = qobject_cast<SimplePimoAnnotation*>( other ) ) {
        return( sa->d->newPropertyLabel == d->newPropertyLabel &&
                SimpleAnnotation::equals( other ) );
    }

    return SimpleAnnotation::equals( other );
}


bool Nepomuk::SimplePimoAnnotation::exists( Resource res ) const
{
    if( d->newPropertyLabel.isEmpty() ) {
        return SimpleAnnotation::exists( res.pimoThing() );
    }
    else {
        return false;
    }
}


void Nepomuk::SimplePimoAnnotation::doCreate( Resource resource )
{
    kDebug() << resource.resourceUri();
    Thing thing = resource.pimoThing();

    if ( !d->newPropertyLabel.isEmpty() ) {
        PimoModel m( ResourceManager::instance()->mainModel() );
        Nepomuk::Types::Property prop
            = m.createProperty( thing.resourceType(),
                                value().isResource()
                                ? value().toResource().resourceType()
                                : Soprano::LiteralValue( value().variant() ).dataTypeUri(),
                                d->newPropertyLabel );
        setProperty( prop );
    }

    SimpleAnnotation::doCreate( thing );
}
