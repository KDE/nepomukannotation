/*
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

#ifndef _NEPOMUK_PIMO_TYPE_ANNOTATION_H_
#define _NEPOMUK_PIMO_TYPE_ANNOTATION_H_

#include "simpleannotation.h"

#include <Nepomuk/Types/Class>
#include <Nepomuk/Types/Property>
#include <Nepomuk/Variant>


namespace Nepomuk {
    class PimoTypeAnnotation : public SimpleAnnotation
    {
        Q_OBJECT

    public:
        PimoTypeAnnotation( QObject* parent );
        ~PimoTypeAnnotation();

        /**
         * Setting a type means that the resource will be typed with this
         * type (or rather its related pimo resource)
         */
        void setType( const Nepomuk::Types::Class& type );

        /**
         * Setting a new type label means that a new type will be
         * created before typing the resource with it.
         */
        void setNewTypeLabel( const QString& s );

        bool exists( Resource ) const;
        bool equals( Annotation* ) const;

        int occurenceCount( const QDateTime& from = QDateTime(), const QDateTime& to = QDateTime() ) const;
        QDateTime lastUsed() const;
        QDateTime firstUsed() const;

    protected:
        void doCreate( Resource resource );

    private:
        Types::Class m_type;
        QString m_newTypeLabel;
    };
}

#endif
