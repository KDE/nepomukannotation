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

#include "textannotationwidget.h"
#include "stackedpimotextannotation.h"

#include <KDebug>

class Nepomuk::TextAnnotationWidget::Private
{
public:
    int offset;
    int length;
    QString text;
};


Nepomuk::TextAnnotationWidget::TextAnnotationWidget( QWidget* parent )
    : AnnotationWidget( parent ),
      d( new Private() )
{
    resetTextExcerpt();
}


Nepomuk::TextAnnotationWidget::~TextAnnotationWidget()
{
    delete d;
}


void Nepomuk::TextAnnotationWidget::setTextExcept( int offset, int length, const QString& text )
{
    d->offset = offset;
    d->length = length;
    d->text = text;
}


void Nepomuk::TextAnnotationWidget::resetTextExcerpt()
{
    d->offset = -1;
    d->length = 0;
    d->text.truncate(0);
}


void Nepomuk::TextAnnotationWidget::createAnnotation( Annotation* anno )
{
    kDebug() << "Creating annotation" << anno;
    if ( d->offset >= 0 && d->length > 0 ) {
        StackedPimoTextAnnotation* sa = new StackedPimoTextAnnotation();
        sa->setTextOffset( d->offset );
        sa->setTextLength( d->length );
        sa->setTextExcerpt( d->text );
        sa->setChildAnnotation( anno );
        sa->create( resource() );
    }
    else {
        AnnotationWidget::createAnnotation( anno );
    }
}

#include "textannotationwidget.moc"
