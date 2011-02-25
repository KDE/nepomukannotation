/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _NEPOMUK_TAG_ANNOTATION_H_
#define _NEPOMUK_TAG_ANNOTATION_H_

#include "annotation.h"
#include "nepomukannotation_export.h"

namespace Nepomuk {

    class Tag;

    /**
     * \brief Simple Annotation class which tags resources.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class NEPOMUKANNOTATION_EXPORT TagAnnotation : public Nepomuk::Annotation
    {
        Q_OBJECT

    public:
        TagAnnotation( QObject* parent = 0 );
        TagAnnotation( const QString& tag, QObject* parent = 0 );
        TagAnnotation( const Tag& tag, QObject* parent = 0 );

        void setTagLabel( const QString& tag );
        void setTag( const Tag& tag );

        /**
         * Constructs a comment from the label
         */
        QString comment() const;

        bool equals( Annotation* other ) const;
        bool exists( Resource res ) const;

    protected:
        void doCreate( Nepomuk::Resource resource );

    private:
        class Private;
        Private* const d;
    };
}

#endif
