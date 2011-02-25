/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>
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

#include "tagannotation.h"

#include <Nepomuk/Tag>

#include <kdebug.h>
#include <klocale.h>
#include <kicon.h>


class Nepomuk::TagAnnotation::Private
{
public:
    QString m_tag;
    QUrl m_tagUri;
};


Nepomuk::TagAnnotation::TagAnnotation( QObject* parent )
    : Annotation( parent ),
      d( new Private() )
{
}


Nepomuk::TagAnnotation::TagAnnotation( const QString& tag, QObject* parent )
    : Annotation( parent ),
      d( new Private() )
{
    setTagLabel( tag );
}


Nepomuk::TagAnnotation::TagAnnotation( const Tag& tag, QObject* parent )
    : Annotation( parent ),
      d( new Private() )
{
    setTag( tag );
}


void Nepomuk::TagAnnotation::setTagLabel( const QString& tag )
{
    d->m_tag = tag;
    d->m_tagUri = QUrl();
    Nepomuk::Resource res( tag );
    setLabel( res.genericLabel() );
    setIcon( KIcon( res.genericIcon() ) );
}


void Nepomuk::TagAnnotation::setTag( const Tag& tag )
{
    // We do not save the resource itself for one simple reason: it would
    // mean that non-existing tags would stay in memory and thus, be
    // handled as existing ones by libnepomuk
    if(tag.exists())
        d->m_tagUri = tag.resourceUri();
    else
        d->m_tag = tag.genericLabel();
    setLabel( tag.genericLabel() );
    setIcon( KIcon( tag.genericIcon() ) );
}


QString Nepomuk::TagAnnotation::comment() const
{
    QString c = Annotation::comment();
    if ( c.isEmpty() )
        c = i18nc( "@action Assign a tag to the selected resource",
                   "Tag with %1", label() );
    return c;
}


bool Nepomuk::TagAnnotation::equals( Annotation* other ) const
{
    if ( TagAnnotation* ta = qobject_cast<TagAnnotation*>( other ) ) {
        return ta->d->m_tag == d->m_tag;
    }
    else
        return Annotation::equals( other );
}


bool Nepomuk::TagAnnotation::exists( Resource res ) const
{
    // FIXME: add Resource::hasTag method
    if( d->m_tagUri.isValid() )
        return res.tags().contains( Nepomuk::Resource( d->m_tagUri ) );
    else
        return res.tags().contains( Nepomuk::Resource( d->m_tag ) );
}


void Nepomuk::TagAnnotation::doCreate( Nepomuk::Resource resource )
{
    kDebug() << "tagging" << resource.resourceUri() << "with" << d->m_tag;
    Nepomuk::Tag tag;
    if( d->m_tagUri.isValid() )
        tag = d->m_tagUri;
    else
        tag = d->m_tag;
    if ( tag.label().isEmpty() &&
         !tag.identifiers().isEmpty() ) {
        tag.setLabel( d->m_tag );
    }
    resource.addTag( tag );
    emitFinished();
}

#include "tagannotation.moc"
