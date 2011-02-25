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

#ifndef _NEPOMUK_SIMPLE_PIMO_ANNOTATION_H_
#define _NEPOMUK_SIMPLE_PIMO_ANNOTATION_H_

#include "simpleannotation.h"

namespace Nepomuk {
    /**
     * \brief Annotates a resource's pimo thing.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class NEPOMUKANNOTATION_EXPORT SimplePimoAnnotation : public SimpleAnnotation
    {
        Q_OBJECT

    public:
        SimplePimoAnnotation( QObject* parent = 0 );
        SimplePimoAnnotation( const Types::Property& prop, const Variant& object, QObject* parent = 0 );
        virtual ~SimplePimoAnnotation();

        /**
         * Setting a new property label means that a new property will be
         * created before annotation the resource with it.
         */
        void setNewPropertyLabel( const QString& s );
        QString newPropertyLabel() const;

        virtual bool equals( Annotation* other ) const;
        virtual bool exists( Resource res ) const;

    protected:
        /**
         * Simply annotates the thing of the set resource with the property
         * and value as passed to the constructor.
         */
        virtual void doCreate( Resource res );

    private:
        class Private;
        Private* const d;
    };
}

#endif
