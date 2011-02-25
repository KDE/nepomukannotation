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

#ifndef _NEPOMUK_RELATION_ITEM_DELEGATE_H_
#define _NEPOMUK_RELATION_ITEM_DELEGATE_H_

#include <kwidgetitemdelegate.h>

namespace Nepomuk {

    class Resource;
    class RelationView;
    class RelationModel;

    class RelationItemDelegate : public KWidgetItemDelegate
    {
        Q_OBJECT

    public:
        RelationItemDelegate( RelationView* view );
        ~RelationItemDelegate();

        void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
        QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const;

        QList<QWidget*> createItemWidgets() const;
        void updateItemWidgets( const QList<QWidget*> widgets,
                                const QStyleOptionViewItem& option,
                                const QPersistentModelIndex& index ) const;

    Q_SIGNALS:
        void resourceEditRequested( const Nepomuk::Resource& res );

    private Q_SLOTS:
        void slotDeleteClicked();
        void slotEditClicked();

    private:
        mutable QSize m_buttonSize;
    };
}

#endif
