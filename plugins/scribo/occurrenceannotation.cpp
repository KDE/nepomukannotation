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

#include "occurrenceannotation.h"
#include "pimo.h"

#include <Nepomuk/Thing>
#include <Nepomuk/Variant>


Nepomuk::OccurrenceAnnotation::OccurrenceAnnotation( QObject* parent )
    : Annotation( parent )
{
}


Nepomuk::OccurrenceAnnotation::OccurrenceAnnotation( const Thing& thing, QObject* parent )
    : Annotation( parent )
{
    setThing( thing );
}


Nepomuk::OccurrenceAnnotation::~OccurrenceAnnotation()
{
}


void Nepomuk::OccurrenceAnnotation::setThing( const Thing& thing )
{
    m_thing = thing;
}


Nepomuk::Thing Nepomuk::OccurrenceAnnotation::thing() const
{
    return m_thing;
}


bool Nepomuk::OccurrenceAnnotation::equals( Annotation* other ) const
{
    if ( OccurrenceAnnotation* oa = qobject_cast<OccurrenceAnnotation*>( other ) )
        return oa->m_thing == m_thing;
    else
        return Annotation::equals( other );
}


bool Nepomuk::OccurrenceAnnotation::exists( Resource res ) const
{
    return( m_thing.occurrences().contains( res ) ||
            m_thing.groundingOccurrences().contains( res ) );
}


void Nepomuk::OccurrenceAnnotation::doCreate( Resource res )
{
    // FIXME: HACK: until we support pimo:occurrence properly
    if ( m_thing.groundingOccurrences().isEmpty() )
        m_thing.addProperty( Nepomuk::Vocabulary::PIMO::groundingOccurrence(), res );
    else
        m_thing.addProperty( Nepomuk::Vocabulary::PIMO::occurrence(), res );
}

#include "occurrenceannotation.moc"
