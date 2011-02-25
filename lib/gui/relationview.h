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

#ifndef _NEPOMUK_RELATION_VIEW_H_
#define _NEPOMUK_RELATION_VIEW_H_

#include <kcategorizedview.h>
#include "nepomukannotation_export.h"

#include "relationmodel.h"

#include <Nepomuk/Thing>

class QItemSelection;

namespace Nepomuk {

    class RelationViewPrivate;

    class NEPOMUKANNOTATION_EXPORT RelationView : public KCategorizedView
    {
        Q_OBJECT

    public:
        RelationView( QWidget* parent = 0 );
        virtual ~RelationView();

    public Q_SLOTS:
        void addRelation( const Nepomuk::RelationModel::Relation& relation );
        void addRelations( const QList<Nepomuk::RelationModel::Relation>& relations );
        void setRelations( const QList<Nepomuk::RelationModel::Relation>& relations );
        void clear();

    Q_SIGNALS:
        void relationSelectionChanged( const QList<Nepomuk::RelationModel::Relation>& );
        void relationActivated( const Nepomuk::RelationModel::Relation& );

        /**
         * Context menu has been requested for a relation at global position pos.
         */
        void contextMenuRequested( const Nepomuk::RelationModel::Relation& relation, const QPoint& pos );

        void resourceEditRequested( const Nepomuk::Resource& res );

    protected:
        void selectionChanged( const QItemSelection& selected, const QItemSelection& deselected );
        void contextMenuEvent( QContextMenuEvent* e );

    private Q_SLOTS:
        void slotActivated( const QModelIndex& );

    private:
        RelationViewPrivate* const d;
    };
}

#endif
