/*
   Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>

   This file is part of the Nepomuk-KDE project.

   Nepomuk-KDE is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   Nepomuk-KDE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Nepomuk-KDE.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _NEPOMUK_TEXT_ANNOTATION_WIDGET_H_
#define _NEPOMUK_TEXT_ANNOTATION_WIDGET_H_

#include "annotationwidget.h"

namespace Nepomuk {
    class TextAnnotationWidget : public AnnotationWidget
    {
        Q_OBJECT

    public:
        TextAnnotationWidget( QWidget* parent = 0 );
        ~TextAnnotationWidget();

    public Q_SLOTS:
        void setTextExcept( int offset, int length, const QString& text );
        void resetTextExcerpt();

    protected:
        void createAnnotation( Annotation* anno );

    private:
        class Private;
        Private* const d;
    };
}

#endif
