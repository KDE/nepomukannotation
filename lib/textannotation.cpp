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

#include "textannotation.h"
#include "nie.h"
#include "nfo.h"
#include "pimo.h"

#include <Nepomuk/Thing>
#include <Nepomuk/Variant>
#include <Nepomuk/ResourceManager>

#include <Soprano/QueryResultIterator>
#include <Soprano/Node>
#include <Soprano/LiteralValue>
#include <Soprano/Model>


class Nepomuk::TextAnnotation::Private
{
public:
    Private()
        : m_offset(0),
          m_length(0) {
    }

    QUrl getTextExcerptResourceUri( Resource res ) const;

    int m_offset;
    int m_length;
    QString m_textExcerpt;
};


QUrl Nepomuk::TextAnnotation::Private::getTextExcerptResourceUri( Resource res ) const
{
    QString query = QString("select ?tr where { "
                            "?tr a %1 . "
                            "?tr %2 %3 . "
                            "?tr <%4textOffset> %5 . "
                            "?tr %6 %7 . "
                            "}")
                    .arg( Soprano::Node::resourceToN3( Vocabulary::NFO::TextDocument() ) )
                    .arg( Soprano::Node::resourceToN3( Vocabulary::NIE::isLogicalPartOf() ) )
                    .arg( Soprano::Node::resourceToN3( res.resourceUri() ) )
                    .arg( Vocabulary::NFO::nfoNamespace().toString() ) // FIXME: add a property to NFO
                    .arg( Soprano::Node::literalToN3( m_offset ) )
                    .arg( Soprano::Node::resourceToN3( Vocabulary::NFO::characterCount() ) )
                    .arg( Soprano::Node::literalToN3( m_length ) );

    Soprano::QueryResultIterator it = ResourceManager::instance()->mainModel()->executeQuery( query, Soprano::Query::QueryLanguageSparql );
    if( it.next() ) {
        // reuse the existing text excerpt resource
        return it["tr"].uri();
    }
    else {
        return QUrl();
    }
}


Nepomuk::TextAnnotation::TextAnnotation( QObject* parent )
    : Annotation( parent ),
      d(new Private())
{
}


Nepomuk::TextAnnotation::~TextAnnotation()
{
    delete d;
}


int Nepomuk::TextAnnotation::textOffset() const
{
    return d->m_offset;
}


int Nepomuk::TextAnnotation::textLength() const
{
    return d->m_length;
}


QString Nepomuk::TextAnnotation::textExcerpt() const
{
    return d->m_textExcerpt;
}


void Nepomuk::TextAnnotation::setTextOffset( int pos )
{
    d->m_offset = pos;
}


void Nepomuk::TextAnnotation::setTextLength( int len )
{
    d->m_length = len;
}


void Nepomuk::TextAnnotation::setTextExcerpt( const QString& text )
{
    d->m_textExcerpt = text;
}


bool Nepomuk::TextAnnotation::equals( Annotation* other ) const
{
    if ( TextAnnotation* ta = qobject_cast<TextAnnotation*>( other ) ) {
        return( ta->d->m_offset == d->m_offset &&
                ta->d->m_length == d->m_length );
    }

    return Annotation::equals( other );
}


bool Nepomuk::TextAnnotation::exists( Resource res ) const
{
    QUrl trUri = d->getTextExcerptResourceUri( res );
    if( trUri.isValid() ) {
        return doesTextAnnotationExist( trUri );
    }
    else {
        // there are situations where the full text is already annotated
        // with the thing in question. Then it does not make sense to do it
        // again, does it?
        return doesTextAnnotationExist( res );
    }
}


// FIXME: if offset is < 0 do annotate the main text resource
// FIXME: should we add a subclass to TextDocument or a completely new class for text excerpts?
void Nepomuk::TextAnnotation::doCreate( Resource res )
{
    // first get the InformationElement in question. We never relate to a thing
    if ( res.hasType( Nepomuk::Vocabulary::PIMO::Thing() ) ) {
        Thing thing( res );
        if ( !thing.groundingOccurrences().isEmpty() )
            res = thing.groundingOccurrences().first();
    }

    // find the resource representing the text excerpt
    // if it does not exist, create it
    // then call doCreateTextAnnotation on it
    QUrl trUri = d->getTextExcerptResourceUri( res );
    if( trUri.isValid() ) {
        // reuse the existing text excerpt resource
        Resource tr( trUri );
        doCreateTextAnnotation( tr );
    }
    else {
        // create a new resource
        Resource tr( QUrl(), Vocabulary::NFO::TextDocument() );
        tr.setProperty( Vocabulary::NIE::isLogicalPartOf(), res );
        res.addProperty( Vocabulary::NIE::hasLogicalPart(), tr );
        if ( !d->m_textExcerpt.isEmpty() )
            tr.setProperty( Vocabulary::NIE::plainTextContent(), d->m_textExcerpt );
        tr.setProperty( QUrl(Vocabulary::NFO::nfoNamespace().toString() + "textOffset"), d->m_offset ); // FIXME: add a property to NFO
        tr.setProperty( Vocabulary::NFO::characterCount(), d->m_length );
        doCreateTextAnnotation( tr );
    }
}

#include "textannotation.moc"
