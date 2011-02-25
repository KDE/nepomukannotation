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

#ifndef _NEPOMUK_TEXT_ANNOTATION_H_
#define _NEPOMUK_TEXT_ANNOTATION_H_

#include "annotation.h"
#include "nepomukannotation_export.h"

namespace Nepomuk {
    /**
     * \class TextAnnotation Nepomuk/TextAnnotation
     *
     * \brief An annotation of a text excerpt.
     *
     * A %TextAnnotation is used to annotate a text at a specific position. It is a
     * convinience class which makes sure an appropriate resource for the text position
     * does exist and then annotates it accordingly. It uses
     * <a href="http://www.semanticdesktop.org/ontologies/2007/01/19/nie#TextDocument">nie:TextDocument</a> as the type
     * for the text excerpt and <a href="http://www.semanticdesktop.org/ontologies/2007/01/19/nie#hasLogicalPart">nie:hasLogicalPart</a> and
     * <a href="http://www.semanticdesktop.org/ontologies/2007/01/19/nie#isLogicalPartOf">nie:isLogicalPartOf</a> to relate the excerpt to the original
     * text resource.
     *
     * Instead of reimplementing Annotation::doCreate() one has to reimplement
     * doCreateTextAnnotation().
     *
     * Instead of reimplementing Annotation::exists() one has to reimplement
     * doesTextAnnotationExist().
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class NEPOMUKANNOTATION_EXPORT TextAnnotation : public Annotation
    {
        Q_OBJECT

    public:
        TextAnnotation( QObject* parent );
        virtual ~TextAnnotation();

        int textOffset() const;
        int textLength() const;
        QString textExcerpt() const;

        virtual bool equals( Annotation* other ) const;
        bool exists( Resource res ) const;

    public Q_SLOTS:
        void setTextOffset( int pos );
        void setTextLength( int len );
        void setTextExcerpt( const QString& text );

    protected:
        /**
         * Reimplement this method to create the actual annotation.
         *
         * \param textRes The resource representing the text excerpt to annotate.
         */
        virtual void doCreateTextAnnotation( Resource textRes ) = 0;

        /**
         * Reimplement this method to check if a text annotation does already exist.
         *
         * \param textRes The resource representing the text excerpt to annotate.
         */
        virtual bool doesTextAnnotationExist( Resource textRes ) const = 0;

    private:
        /**
         * Made private in favor of doCreateTextAnnotation
         */
        void doCreate( Resource res );

        class Private;
        Private* const d;
    };
}

#endif
