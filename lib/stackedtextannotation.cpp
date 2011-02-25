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

#include "stackedtextannotation.h"

#include <QtGui/QIcon>


class Nepomuk::StackedTextAnnotation::Private
{
public:
    Annotation* m_childAnnotation;
};

Nepomuk::StackedTextAnnotation::StackedTextAnnotation( QObject* parent )
    : TextAnnotation( parent ),
      d(new Private())
{
    d->m_childAnnotation = 0;
}


Nepomuk::StackedTextAnnotation::~StackedTextAnnotation()
{
    delete d->m_childAnnotation;
    delete d;
}


QString Nepomuk::StackedTextAnnotation::label() const
{
    if( !TextAnnotation::label().isEmpty() )
        return TextAnnotation::label();
    else if( d->m_childAnnotation )
        return d->m_childAnnotation->label();
    else
        return QString();
}


QString Nepomuk::StackedTextAnnotation::comment() const
{
    if( !TextAnnotation::comment().isEmpty() )
        return TextAnnotation::comment();
    else if( d->m_childAnnotation )
        return d->m_childAnnotation->comment();
    else
        return QString();
}


QIcon Nepomuk::StackedTextAnnotation::icon() const
{
    if( !TextAnnotation::icon().isNull() )
        return TextAnnotation::icon();
    else if( d->m_childAnnotation )
        return d->m_childAnnotation->icon();
    else
        return QIcon();
}

qreal Nepomuk::StackedTextAnnotation::relevance() const
{
    if( TextAnnotation::relevance() > 0.0 )
        return TextAnnotation::relevance();
    else if( d->m_childAnnotation )
        return d->m_childAnnotation->relevance();
    else
        return 0.0;
}


Nepomuk::Annotation* Nepomuk::StackedTextAnnotation::childAnnotation() const
{
    return d->m_childAnnotation;
}


void Nepomuk::StackedTextAnnotation::setChildAnnotation( Annotation* anno )
{
    if( d->m_childAnnotation != anno )
        delete d->m_childAnnotation;
    d->m_childAnnotation = anno;
    connect( d->m_childAnnotation, SIGNAL(finished( Nepomuk::Annotation* )),
             this, SIGNAL(finished( Nepomuk::Annotation* )) );
}


void Nepomuk::StackedTextAnnotation::doCreateTextAnnotation( Resource res )
{
    d->m_childAnnotation->create( res );
}


bool Nepomuk::StackedTextAnnotation::doesTextAnnotationExist( Resource textRes ) const
{
    return d->m_childAnnotation->exists( textRes );
}


bool Nepomuk::StackedTextAnnotation::equals( Annotation* other ) const
{
    if ( StackedTextAnnotation* sta = qobject_cast<StackedTextAnnotation*>( other ) )
        return sta->d->m_childAnnotation->equals( d->m_childAnnotation );
    else
        return TextAnnotation::equals( other );
}

#include "stackedtextannotation.moc"
