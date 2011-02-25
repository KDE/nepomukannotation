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

#ifndef _NEPOMUK_STACKED_TEXT_ANNOTATION_H_
#define _NEPOMUK_STACKED_TEXT_ANNOTATION_H_

#include "textannotation.h"
#include "nepomukannotation_export.h"

namespace Nepomuk {
    /**
     * \class StackedTextAnnotation Nepomuk/StackedTextAnnotation
     *
     * \brief A TextAnnotation that is wrapped around a "normal" Annotation.
     *
     * StackedTextAnnotation allows to reuse "normal" non-text Annotation instances
     * to annotate a text excerpt. This allows to reuse the power of convinience classes
     * like SimpleAnnotation or TagAnnotation without having to reimplement them as
     * subclasses of TextAnnotation.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class NEPOMUKANNOTATION_EXPORT StackedTextAnnotation : public TextAnnotation
    {
        Q_OBJECT

    public:
        StackedTextAnnotation( QObject* parent = 0 );
        virtual ~StackedTextAnnotation();

        /**
         * A short label to show to the user.
         *
         * Defaults to the value of the child annotation if empty.
         */
        virtual QString label() const;

        /**
         * A longer description to show to the user.
         *
         * Defaults to the value of the child annotation if empty.
         */
        virtual QString comment() const;

        /**
         * An icon to display to the user.
         *
         * Defaults to the value of the child annotation if empty.
         */
        virtual QIcon icon() const;

        /**
         * The relevance of the annotation between 0 and 1.
         *
         * The relevance should not take statistics into account. See
         * occurenceCount, lastUsed for statistics.
         *
         * Defaults to the value of the child annotation if empty.
         */
        virtual qreal relevance() const;

        virtual bool equals( Annotation* other ) const;

        /**
         * \return The child annotation set via setChildAnnotation
         */
        Annotation* childAnnotation() const;

    public Q_SLOTS:
        /**
         * Set the child annotation which will be used to create an annotation
         * on the text excerpt. StackedTextAnnotation will take ownership or \a anno.
         */
        void setChildAnnotation( Annotation* anno );

    protected:
        virtual void doCreateTextAnnotation( Resource res );
        virtual bool doesTextAnnotationExist( Resource textRes ) const;

    private:
        class Private;
        Private* const d;
    };
}

#endif
