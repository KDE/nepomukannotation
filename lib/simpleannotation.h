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

#ifndef _NEPOMUK_SIMPLE_ANNOTATION_H_
#define _NEPOMUK_SIMPLE_ANNOTATION_H_

#include "annotation.h"
#include "nepomukannotation_export.h"

class QUrl;

namespace Nepomuk {

    class Variant;
    namespace Types {
        class Property;
    }

    /**
     * \brief Annotation class that creates an arbitrary relation.
     *
     * The SimpleAnnotation uses a Nepomuk::Types::Property and a Nepomuk::Variant
     * as value to create a new annotation for a resource.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class NEPOMUKANNOTATION_EXPORT SimpleAnnotation : public Annotation
    {
        Q_OBJECT

    public:
        SimpleAnnotation( QObject* parent = 0 );
        SimpleAnnotation( const Types::Property& prop, const Variant& object, QObject* parent = 0 );
        virtual ~SimpleAnnotation();

        /**
         * Will also set the icon using Types::Property::icon()
         */
        void setProperty( const Types::Property& );

        void setValue( const Variant& value );

        Types::Property property() const;
        Variant value() const;

        virtual bool equals( Annotation* other ) const;
        virtual bool exists( Resource res ) const;

        virtual int occurenceCount( const QDateTime& from = QDateTime(), const QDateTime& to = QDateTime() ) const;
        virtual QDateTime lastUsed() const;
        virtual QDateTime firstUsed() const;

    protected:
        /**
         * Simply annotates the resource with the property
         * and value as passed to the constructor.
         */
        virtual void doCreate( Resource res );

    private:
        class Private;
        Private* const d;
    };
}

#endif
