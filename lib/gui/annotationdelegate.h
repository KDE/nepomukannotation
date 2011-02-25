/*
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

#ifndef _NEPOMUK_ANNOTATION_DELEGATE_H_
#define _NEPOMUK_ANNOTATION_DELEGATE_H_

#include <QtGui/QStyledItemDelegate>

#include "nepomukannotation_export.h"


namespace Nepomuk {

    class Resource;

    class NEPOMUKANNOTATION_EXPORT AnnotationDelegate : public QStyledItemDelegate
    {
        Q_OBJECT

    public:
        AnnotationDelegate( QObject* parent = 0 );
        ~AnnotationDelegate();

        void setResource( const Nepomuk::Resource& );

        void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
        QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const;
        bool editorEvent( QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index );

    private:
        class Private;
        Private* const d;
    };
}

#endif
