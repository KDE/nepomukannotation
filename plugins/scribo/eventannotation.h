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

#ifndef _NEPOMUK_EVENT_ANNOTATION_H_
#define _NEPOMUK_EVENT_ANNOTATION_H_

#include "textannotation.h"
#include <scribo/statement.h>
#include <scribo/entity.h>

#include <QtCore/QList>


namespace Nepomuk {
    class EventAnnotation : public TextAnnotation
    {
        Q_OBJECT

    public:
        EventAnnotation( QObject* parent = 0 );
        ~EventAnnotation();

        QString label() const;
        QString comment() const;

        bool equals( Annotation* other ) const;
        bool exists( Resource res ) const;

        void setDates( const QList<Scribo::Statement>& dates ) {
            m_dates = dates;
        }
        void setLocations( const QList<Scribo::Entity>& locations ) {
            m_locations = locations;
        }
        void setProjects( const QList<Scribo::Entity>& projects ) {
            m_projects = projects;
        }

    protected:
        // we use doCreate instead of doCreateTextAnnotation since we
        // do not actually want to annotate the text, this is more of an action
        void doCreate( Resource res );

        void doCreateTextAnnotation( Resource ) {}
        bool doesTextAnnotationExist( Resource ) const { return false; }

    private:
        QList<Scribo::Statement> m_dates;
        QList<Scribo::Entity> m_locations;
        QList<Scribo::Entity> m_projects;
    };
}

#endif
