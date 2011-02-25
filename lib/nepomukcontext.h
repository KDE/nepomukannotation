/* This file is part of the Nepomuk Project
   Copyright (c) 2009-2011 Sebastian Trueg <trueg@kde.org>

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

#ifndef _NEPOMUK_CONTEXT_H_
#define _NEPOMUK_CONTEXT_H_

#include <nepomuk/resource.h>
#include "nepomukannotation_export.h"

namespace Nepomuk {
    class NEPOMUKANNOTATION_EXPORT ContextServiceInterface : public QObject
    {
        Q_OBJECT

    public:
        ContextServiceInterface( QObject* parent = 0 );
        ~ContextServiceInterface();

        Nepomuk::Resource currentContext();
        void setCurrentContext( const Nepomuk::Resource& res );

        Nepomuk::Resource currentProject();
        void setCurrentProject( const Nepomuk::Resource& res );

        Nepomuk::Resource currentTask();
        void setCurrentTask( const Nepomuk::Resource& res );

        static bool isAvailable();
        static ContextServiceInterface* instance();

    Q_SIGNALS:
        void currentContextChanged( const Nepomuk::Resource& res );
        void currentProjectChanged( const Nepomuk::Resource& res );
        void currentTaskChanged( const Nepomuk::Resource& res );

    private Q_SLOTS:
        void slotContextChanged( const QString& s );
        void slotProjectChanged( const QString& s );
        void slotTaskChanged( const QString& s );

    private:
        class Private;
        Private* const d;
    };
}

#endif
