/* This file is part of the Nepomuk Project
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

#include "resourceactionplugin.h"

#include <QtCore/QTimer>


class Nepomuk::ResourceActionPlugin::Private
{
public:
    Private( ResourceActionPlugin* parent )
        : q( parent ) {
    }

    // used for async calling of doGetPossibleActions
    QUrl possibleActionResource;
    ActionTypes possibleActionTypes;
    QString possibleActionFilter;

    void slotGetPossibleActions();

private:
    ResourceActionPlugin* q;
};


void Nepomuk::ResourceActionPlugin::Private::slotGetPossibleActions()
{
    q->doGetPossibleActions( possibleActionResource, possibleActionTypes, possibleActionFilter );
}



Nepomuk::ResourceActionPlugin::ResourceActionPlugin(QObject* parent)
    : QObject(parent),
      d( new Private(this) )
{
}


Nepomuk::ResourceActionPlugin::~ResourceActionPlugin()
{
    delete d;
}


void Nepomuk::ResourceActionPlugin::getPossibleActions( const QUrl& resource, ActionTypes types, const QString& filter )
{
    d->possibleActionResource = resource;
    d->possibleActionFilter = filter;
    d->possibleActionTypes = types;
    QTimer::singleShot( 0, this, SLOT(slotGetPossibleActions()) );
}


void Nepomuk::ResourceActionPlugin::doGetPossibleActions( const QUrl& resource, ActionTypes types, const QString& filter )
{
    Q_UNUSED( resource );
    Q_UNUSED( types );
    Q_UNUSED( filter );

    // nothing to list in the default impl
    emit finished();
}

#include "resourceactionplugin.moc"
