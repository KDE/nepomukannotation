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

#ifndef _NEPOMUK_OCCURRENCE_ANNOTATION_H_
#define _NEPOMUK_OCCURRENCE_ANNOTATION_H_

#include "annotation.h"

#include <Nepomuk/Thing>

namespace Nepomuk {
    /**
     * \brief Annotation class that creates a pimo:occurrence relation.
     *
     * The resource passed into create() will be set as occurrence of the
     * configured Thing.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class OccurrenceAnnotation : public Annotation
    {
        Q_OBJECT

    public:
        OccurrenceAnnotation( QObject* parent = 0 );
        OccurrenceAnnotation( const Thing& thing, QObject* parent = 0 );
        ~OccurrenceAnnotation();

        void setThing( const Thing& value );

        Thing thing() const;

        bool equals( Annotation* other ) const;
        bool exists( Resource res ) const;

    protected:
        void doCreate( Resource res );

    private:
        Thing m_thing;
    };
}

#endif
