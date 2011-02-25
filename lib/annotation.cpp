/* This file is part of the Nepomuk Project
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

#include "annotation.h"
#include "annotation_p.h"

#include <QtGui/QIcon>
#include <QtCore/QUrl>

#include <kdebug.h>

Nepomuk::Annotation::Annotation( QObject* parent )
    : QObject( parent ),
      d( new AnnotationPrivate() )
{
}


Nepomuk::Annotation::~Annotation()
{
    delete d;
}


QString Nepomuk::Annotation::label() const
{
    return d->label;
}


QString Nepomuk::Annotation::comment() const
{
    return d->comment;
}


QIcon Nepomuk::Annotation::icon() const
{
    return d->icon;
}


qreal Nepomuk::Annotation::relevance() const
{
    return d->relevance;
}


void Nepomuk::Annotation::create( Resource res )
{
    kDebug() << res.resourceUri();
    // call a protected method to be open for future extensions at
    // this place.
    doCreate( res );
}

void Nepomuk::Annotation::setLabel( const QString& label )
{
    d->label = label;
}


void Nepomuk::Annotation::setComment( const QString& comment )
{
    d->comment = comment;
}


void Nepomuk::Annotation::setIcon( const QIcon& icon )
{
    d->icon = icon;
}


void Nepomuk::Annotation::setRelevance( qreal r )
{
    d->relevance = qBound(qreal(0.0) , r, qreal(1.0));
}


void Nepomuk::Annotation::emitFinished()
{
    emit finished( this );
}


int Nepomuk::Annotation::occurenceCount( const QDateTime&, const QDateTime& ) const
{
    return -1;
}


bool Nepomuk::Annotation::equals( Annotation* other ) const
{
    // the stupid default comparision.
    return this == other;
}


QDateTime Nepomuk::Annotation::lastUsed() const
{
    return QDateTime();
}


QDateTime Nepomuk::Annotation::firstUsed() const
{
    return QDateTime();
}


void Nepomuk::Annotation::setUserRelevance( qreal r )
{
    Q_UNUSED( r );
}

#include "annotation.moc"
