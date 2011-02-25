/* This file is part of the Nepomuk Project
   Copyright (c) 2009-2011 Sebastian Trueg <trueg@kde.org>

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

#include "eventannotation.h"
#include "simpleannotation.h"
#include "resourceeditor.h"

#include <KGlobal>
#include <KLocale>
#include <KDialog>

#include <Nepomuk/Types/Class>
#include <Nepomuk/Types/Property>
#include <Nepomuk/Variant>
#include <Nepomuk/Thing>
#include <Nepomuk/Vocabulary/PIMO>


Nepomuk::EventAnnotation::EventAnnotation( QObject* parent )
    : TextAnnotation( parent )
{
}


Nepomuk::EventAnnotation::~EventAnnotation()
{
}


QString Nepomuk::EventAnnotation::label() const
{
    return i18n( "Create Event" );
}


QString Nepomuk::EventAnnotation::comment() const
{
    if ( m_locations.count() == 1 )
        return i18n( "Create event in %1", m_locations.first().label() );
    else
        return label();
}


bool Nepomuk::EventAnnotation::equals( Annotation* other ) const
{
    return TextAnnotation::equals( other );
}


bool Nepomuk::EventAnnotation::exists( Resource res ) const
{
    // we ignore it, this whole class is a hack anyway
    return false;
}


void Nepomuk::EventAnnotation::doCreate( Resource res )
{
    // now this is where we open a dialog that proposes to create a new event and a bunch of new annotations based
    // on the info in our TextMatches
    QList<Annotation*> annotations;
    foreach( const Scribo::Statement& ds, m_dates ) {
        SimpleAnnotation* anno = new SimpleAnnotation();
        anno->setProperty( ds.property() );
        anno->setValue( ds.value() );
        anno->setLabel( "Date" );
        anno->setComment( i18n( "Set %1 to %2", ds.property().label(), KGlobal::locale()->formatDate( ds.value().toDate() ) ) );
        annotations << anno;
    }
    foreach( const Scribo::Entity& entity, m_locations ) {
        SimpleAnnotation* anno = new SimpleAnnotation();
        anno->setProperty( Nepomuk::Vocabulary::PIMO::hasLocation() );
        if ( entity.localResource().isValid() )
            anno->setValue( entity.localResource() );
        else
            anno->setValue( Thing( entity.label(), entity.type().uri() ) );
        anno->setLabel( entity.label() );
        anno->setComment( i18n( "Is located in %1", entity.label() ) );
        annotations << anno;
    }
    foreach( const Scribo::Entity& entity, m_projects ) {
        if ( entity.localResource().isValid() ) {
            SimpleAnnotation* anno = new SimpleAnnotation();
            anno->setProperty( Nepomuk::Vocabulary::PIMO::isRelated() );
            anno->setValue( entity.localResource() );
            anno->setLabel( entity.label() );
            anno->setComment( i18n( "Is related to %1 (%2)", entity.localResource().genericLabel(), Types::Class( entity.localResource().resourceType() ).label() ) );
            annotations << anno;
        }
    }

    Thing event( QUrl(), Nepomuk::Vocabulary::PIMO::Event() );

    // now create the dialog that allows to create an event
    KDialog dlg;
    ResourceEditor* re = new ResourceEditor( &dlg );
    dlg.setMainWidget( re );
    dlg.setButtons( KDialog::Close );
    dlg.setCaption( i18n( "Create new Event" ) );
    re->setResource( event, annotations );
    dlg.exec();

    if ( event.exists() )
        res.pimoThing().addProperty( Nepomuk::Vocabulary::PIMO::isRelated(), event );
}

#include "eventannotation.moc"
