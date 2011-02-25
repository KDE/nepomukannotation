/* This file is part of the Nepomuk Project
   Copyright (c) 2008-2010 Sebastian Trueg <trueg@kde.org>

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

#ifndef _NEPOMUK_USER_CONTEXT_SERVICE_H_
#define _NEPOMUK_USER_CONTEXT_SERVICE_H_

#include <Nepomuk/Service>
#include <Nepomuk/Resource>

#include <QtCore/QVariant>


namespace Nepomuk {
    /**
     * Maintains the current project (pimo:Project) and the current task
     * in that project (pimo:Task).
     *
     * If no project is set this means we are working in the default project
     * which by definition all tasks that are not related to a project are in.
     *
     * In addition to the current project and task the service maintains a
     * current context which can be anything. It could be a file, a person,
     * an email, and so on.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class UserContextService : public Service
    {
        Q_OBJECT

    public:
        /**
         * both paramters are basically unused but forced by the KDE
         * plugin system.
         */
        UserContextService( QObject* parent, const QVariantList& );
        ~UserContextService();

    public Q_SLOTS:
        /**
         * \return The URI of the currently active user context. An emtpy
         * string if no context is active (ie. the anonymeous context)
         */
        QString currentUserContext();
        QString currentProject();
        QString currentTask();

        void setCurrentUserContext( const QString& uri );

        /**
         * Set the current project. This will also reset the current task to none.
         */
        void setCurrentProject( const QString& uri );

        /**
         * Will set the current task to \p uri.
         * If the task is not related to the current project (if any) yet this link
         * will be created.
         */
        void setCurrentTask( const QString& uri );

    Q_SIGNALS:
        void currentUserContextChanged( const QString& uri );
        void currentProjectChanged( const QString& uri );
        void currentTaskChanged( const QString& uri );

    private:
        bool isPartOf( const Resource& parent, const Resource& potentialChild );

        Resource m_currentContext;
        Resource m_currentProject;
        Resource m_currentTask;
    };
}

#endif
