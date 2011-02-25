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

#ifndef _NEPOMUK_STACKED_PIMO_TEXT_ANNOTATION_H_
#define _NEPOMUK_STACKED_PIMO_TEXT_ANNOTATION_H_

#include "stackedtextannotation.h"
#include "nepomukannotation_export.h"

namespace Nepomuk {
    /**
     * \class StackedPimoTextAnnotation Nepomuk/StackedPimoTextAnnotation
     *
     * \brief A StackedTextAnnotation that annotates the pimo thing related to the text.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class NEPOMUKANNOTATION_EXPORT StackedPimoTextAnnotation : public StackedTextAnnotation
    {
        Q_OBJECT

    public:
        StackedPimoTextAnnotation( QObject* parent = 0 );
        ~StackedPimoTextAnnotation();

        bool equals( Annotation* other ) const;

    protected:
        void doCreateTextAnnotation( Resource res );
        bool doesTextAnnotationExist( Resource textRes ) const;

    private:
        class Private;
        Private* const d;
    };
}

#endif
