/* This file is part of the Nepomuk Project
   Copyright (c) 2008-2011 Sebastian Trueg <trueg@kde.org>

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

#include "nepomukusercontextservice.h"
#include "usercontextserviceadaptor.h"

#include <KUrl>
#include <KLocale>
#include <KPluginFactory>
#include <KDebug>

#include <Soprano/Model>
#include <Soprano/StatementIterator>
#include <Soprano/Statement>
#include <Soprano/Node>
#include <Soprano/NodeIterator>
#include <Soprano/QueryResultIterator>

#include <Nepomuk/Resource>
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/PIMO>


/**
 * this marco creates a service factory which can then be found by the Qt/KDE
 * plugin system in the Nepomuk server.
 */
NEPOMUK_EXPORT_SERVICE( Nepomuk::UserContextService, "nepomukusercontextservice")


// TODO: once a task or project is completed we should also remove it as a context automatically

Nepomuk::UserContextService::UserContextService( QObject* parent, const QList<QVariant>& )
    : Service( parent )
{
    // only so ResourceManager won't open yet another connection to the nepomuk server
    ResourceManager::instance()->setOverrideMainModel( mainModel() );

    ( void )new UserContextServiceAdaptor( this );
}


Nepomuk::UserContextService::~UserContextService()
{
}


QString Nepomuk::UserContextService::currentUserContext()
{
    return KUrl( m_currentContext.resourceUri() ).url();
}


QString Nepomuk::UserContextService::currentProject()
{
    return KUrl( m_currentProject.resourceUri() ).url();
}


QString Nepomuk::UserContextService::currentTask()
{
    return KUrl( m_currentTask.resourceUri() ).url();
}


void Nepomuk::UserContextService::setCurrentUserContext( const QString& uriString )
{
    KUrl uri( uriString );
    if ( uriString.isEmpty() ) {
        m_currentContext = Resource();
        emit currentUserContextChanged( uriString );
    }
    else if ( uri != m_currentContext.resourceUri() ) {
        m_currentContext = uri;
        m_currentContext.increaseUsageCount();
        emit currentUserContextChanged( uriString );
    }
}


void Nepomuk::UserContextService::setCurrentProject( const QString& uriString )
{
    KUrl uri( uriString );
    if ( uriString.isEmpty() ) {
        m_currentProject = Resource();
        emit currentProjectChanged( uriString );
    }
    else if ( m_currentProject != uri ) {
        m_currentProject = uri;
        if ( !m_currentProject.hasType( Vocabulary::PIMO::Project() ) )
            m_currentProject.addType( Vocabulary::PIMO::Project() );

        // check if the current task is part of the new project and if not
        // reset it
        if ( m_currentTask.isValid() &&
             !isPartOf( m_currentProject, m_currentTask ) ) {
            m_currentTask = Resource();
            emit currentTaskChanged( QString() );
        }

        emit currentProjectChanged( uriString );
    }
}


void Nepomuk::UserContextService::setCurrentTask( const QString& uriString )
{
    KUrl uri( uriString );
    if ( uriString.isEmpty() ) {
        m_currentTask = Resource();
        emit currentTaskChanged( uriString );
    }
    else if ( m_currentTask != uri ) {
        m_currentTask = uri;
        if ( !m_currentTask.hasType( Vocabulary::PIMO::Task() ) )
            m_currentTask.addType( Vocabulary::PIMO::Task() );
        emit currentTaskChanged( uriString );
    }
}


bool Nepomuk::UserContextService::isPartOf( const Resource& parent, const Resource& potentialChild )
{
    // FIXME
    return true;
}

#include "nepomukusercontextservice.moc"
