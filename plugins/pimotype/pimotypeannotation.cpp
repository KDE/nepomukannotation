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

#include "pimotypeannotation.h"
#include "pimomodel.h"
#include "pimo.h"

#include <Nepomuk/Thing>
#include <Nepomuk/Variant>
#include <Nepomuk/ResourceManager>

#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Soprano/Vocabulary/RDF>
#include <Soprano/Vocabulary/RDFS>
#include <Soprano/Vocabulary/NAO>

#include <KIcon>


Nepomuk::PimoTypeAnnotation::PimoTypeAnnotation( QObject* parent )
    : SimpleAnnotation( parent )
{
    setProperty( Soprano::Vocabulary::RDF::type() );
}


Nepomuk::PimoTypeAnnotation::~PimoTypeAnnotation()
{
}


void Nepomuk::PimoTypeAnnotation::setType( const Nepomuk::Types::Class& type )
{
    m_type = type;
    if ( m_type.icon().isNull() )
        setIcon( KIcon( "nepomuk" ) );
    else
        setIcon( m_type.icon() );

    // SimpleAnnotation needs this for statistics
    setValue( type.uri() );
}



void Nepomuk::PimoTypeAnnotation::setNewTypeLabel( const QString& s )
{
    m_newTypeLabel = s;
    setIcon( KIcon( "document-new" ) );
}


void Nepomuk::PimoTypeAnnotation::doCreate( Resource resource )
{
    if ( !m_newTypeLabel.isEmpty() ) {
        PimoModel m( ResourceManager::instance()->mainModel() );
        m_type = m.createClass( Vocabulary::PIMO::Thing(),
                                m_newTypeLabel );
    }

    resource.pimoThing().addType( m_type );
    emitFinished();
}


bool Nepomuk::PimoTypeAnnotation::exists( Resource resource ) const
{
    return( resource.pimoThing().isValid() &&
            m_type.isValid() &&
            resource.pimoThing().hasType( m_type.uri() ) );
}


bool Nepomuk::PimoTypeAnnotation::equals( Annotation* other ) const
{
    if ( PimoTypeAnnotation* ta = qobject_cast<PimoTypeAnnotation*>( other ) ) {
        return( ta->m_type == m_type &&
                ta->m_newTypeLabel == m_newTypeLabel );
    }
    else {
        return SimpleAnnotation::equals( other );
    }
}


int Nepomuk::PimoTypeAnnotation::occurenceCount( const QDateTime& from, const QDateTime& to ) const
{
    //
    // the idea is this:
    // existing types we handle like the SimpleAnnotation does: just count how often it has been used
    // for creating new types, we count how often the user created a type. So far the only possibility
    // to differenciate between pimo properties and user-created ones is the nao:created date.
    //
    if( !m_newTypeLabel.isEmpty() ) {
        QString query = QString( "?r a %1 . ?r %2 ?date . ")
                        .arg( Soprano::Node( Soprano::Vocabulary::RDFS::Class() ).toN3() )
                        .arg( Soprano::Node( Soprano::Vocabulary::NAO::created() ).toN3() );

        if( from.isValid() ) {
            query += QString("FILTER(?date >= %1) . ")
                     .arg( Soprano::Node(from).toN3() );
        }
        if( to.isValid() ) {
            query += QString("FILTER(?date <= %1) . ")
                     .arg( Soprano::Node(to).toN3() );
        }

        query = "select ?r where { " + query + "}";

        // why oh why doesn't sparql have a count method?
        Soprano::QueryResultIterator it = ResourceManager::instance()->mainModel()->executeQuery( query, Soprano::Query::QueryLanguageSparql );
        int cnt = 0;
        while( it.next() ) {
            ++cnt;
        }
        return cnt;
    }
    else {
        return SimpleAnnotation::occurenceCount( from, to );
    }
}


QDateTime Nepomuk::PimoTypeAnnotation::lastUsed() const
{
    // see occurenceCount for the idea
    if( !m_newTypeLabel.isEmpty() ) {
        Soprano::QueryResultIterator it =
            ResourceManager::instance()->mainModel()->executeQuery( QString("select ?date where { ?r a %1 . ?r %2 ?date } ORDER BY DESC(?date) LIMIT 1")
                                                                    .arg( Soprano::Node( Soprano::Vocabulary::RDFS::Class() ).toN3() )
                                                                    .arg( Soprano::Node( Soprano::Vocabulary::NAO::created() ).toN3() ),
                                                                    Soprano::Query::QueryLanguageSparql );
        if( it.next() ) {
            return it.binding( "date" ).literal().toDateTime();
        }
        else {
            return QDateTime();
        }
    }
    else {
        return SimpleAnnotation::lastUsed();
    }
}


QDateTime Nepomuk::PimoTypeAnnotation::firstUsed() const
{
    // see occurenceCount for the idea
    if( !m_newTypeLabel.isEmpty() ) {
        Soprano::QueryResultIterator it =
            ResourceManager::instance()->mainModel()->executeQuery( QString("select ?date where { ?r a %1 . ?r %2 ?date } ORDER BY ASC(?date) LIMIT 1")
                                                                    .arg( Soprano::Node( Soprano::Vocabulary::RDFS::Class() ).toN3() )
                                                                    .arg( Soprano::Node( Soprano::Vocabulary::NAO::created() ).toN3() ),
                                                                    Soprano::Query::QueryLanguageSparql );
        if( it.next() ) {
            return it.binding( "date" ).literal().toDateTime();
        }
        else {
            return QDateTime();
        }
    }
    else {
        return SimpleAnnotation::firstUsed();
    }
}

#include "pimotypeannotation.moc"
