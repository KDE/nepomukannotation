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

#ifndef _NEPOMUK_ANNOTATION_WIDGET_H_
#define _NEPOMUK_ANNOTATION_WIDGET_H_

#include <QtGui/QWidget>
#include <QtCore/QUrl>

#include "nepomukannotation_export.h"

#include <nepomuk/resource.h>

namespace Nepomuk {

    class Annotation;

    class NEPOMUKANNOTATION_EXPORT AnnotationWidget : public QWidget
    {
        Q_OBJECT

    public:
        AnnotationWidget( QWidget* parent = 0 );
        virtual ~AnnotationWidget();

        bool eventFilter( QObject* watched, QEvent* event );

        Resource resource() const;

    Q_SIGNALS:
        /**
         * Emitted if an annotation was added using a plugin.
         *
         * \param resource The resource which was changed.
         */
        void annotationAdded( const Nepomuk::Resource& resource );

    public Q_SLOTS:
        void setResource( const Nepomuk::Resource& resource );

    protected:
        /**
         * When the user executes an annotation by double clicking or
         * pressing enter the annotation is removed from the list and
         * created via this method. The default method simply calls
         * Annotation::create on the current annotation.
         */
        virtual void createAnnotation( Nepomuk::Annotation* anno );

    private:
        class Private;
        Private* const d;

        Q_PRIVATE_SLOT( d, void _k_slotAnnotationFinished( Nepomuk::Annotation* anno ) )
        Q_PRIVATE_SLOT( d, void _k_slotExecuteAnnotation( Nepomuk::Annotation* anno ) )
        Q_PRIVATE_SLOT( d, void _k_slotFilterChanged( const QString& filter ) )
        Q_PRIVATE_SLOT( d, void _k_slotAnnotationsChanged() )
    };
}

#endif
