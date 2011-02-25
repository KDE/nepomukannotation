/* This file is part of the Nepomuk Project
   Copyright (c) 2009-2010 Sebastian Trueg <trueg@kde.org>

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

#include "nepomukcontext.h"
#include "nepomukcontextserviceinterface.h"
#include "dbusconnectionpool.h"

#include <KUrl>

namespace {
    class ContextServiceDBusInterface : public org::kde::nepomuk::UserContextService
    {
    public:
        ContextServiceDBusInterface()
            : org::kde::nepomuk::UserContextService( QLatin1String( "org.kde.nepomuk.services.nepomukusercontextservice" ),
                                                     QLatin1String( "/nepomukusercontextservice" ),
                                                     DBusConnectionPool::threadConnection(),
                                                     0 ) {
        }
    };
}


class Nepomuk::ContextServiceInterface::Private
{
public:
    ContextServiceDBusInterface interface;
};


Nepomuk::ContextServiceInterface::ContextServiceInterface( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
    connect( &d->interface, SIGNAL( currentUserContextChanged( QString ) ),
             this, SLOT( slotContextChanged( QString ) ) );
    connect( &d->interface, SIGNAL( currentProjectChanged( QString ) ),
             this, SLOT( slotProjectChanged( QString ) ) );
    connect( &d->interface, SIGNAL( currentTaskChanged( QString ) ),
             this, SLOT( slotTaskChanged( QString ) ) );
}


Nepomuk::ContextServiceInterface::~ContextServiceInterface()
{
    delete d;
}


Nepomuk::Resource Nepomuk::ContextServiceInterface::currentContext()
{
    return KUrl( d->interface.currentUserContext().value() );
}


Nepomuk::Resource Nepomuk::ContextServiceInterface::currentProject()
{
    return KUrl( d->interface.currentProject().value() );
}


Nepomuk::Resource Nepomuk::ContextServiceInterface::currentTask()
{
    return KUrl( d->interface.currentTask().value() );
}


void Nepomuk::ContextServiceInterface::setCurrentContext( const Nepomuk::Resource& res )
{
    d->interface.setCurrentUserContext( KUrl( res.resourceUri() ).url() );
}


void Nepomuk::ContextServiceInterface::setCurrentProject( const Nepomuk::Resource& res )
{
    d->interface.setCurrentProject( KUrl( res.resourceUri() ).url() );
}


void Nepomuk::ContextServiceInterface::setCurrentTask( const Nepomuk::Resource& res )
{
    d->interface.setCurrentTask( KUrl( res.resourceUri() ).url() );
}


void Nepomuk::ContextServiceInterface::slotContextChanged( const QString& s )
{
    emit currentContextChanged( KUrl( s ) );
}


void Nepomuk::ContextServiceInterface::slotProjectChanged( const QString& s )
{
    emit currentProjectChanged( KUrl( s ) );
}


void Nepomuk::ContextServiceInterface::slotTaskChanged( const QString& s )
{
    emit currentTaskChanged( KUrl( s ) );
}


// static
bool Nepomuk::ContextServiceInterface::isAvailable()
{
    ContextServiceDBusInterface si;
    return si.isValid();
}


Q_GLOBAL_STATIC( Nepomuk::ContextServiceInterface, s_contextServiceInterface )
// static
Nepomuk::ContextServiceInterface* Nepomuk::ContextServiceInterface::instance()
{
    return s_contextServiceInterface();
}
