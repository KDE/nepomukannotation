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

#ifndef _NEPOMUK_THING_VIEW_H_
#define _NEPOMUK_THING_VIEW_H_

#include <kcategorizedview.h>
#include "nepomukannotation_export.h"

#include <Nepomuk/Thing>

class QItemSelection;

namespace Nepomuk {
    // FIXME: rename this to RelationView
    class NEPOMUKANNOTATION_EXPORT ThingView : public KCategorizedView
    {
        Q_OBJECT

    public:
        ThingView( QWidget* parent = 0 );
        virtual ~ThingView();

    public Q_SLOTS:
        void addThing( const Nepomuk::Thing& thing );
        void addThings( const QList<Nepomuk::Thing>& things );
        void setThings( const QList<Nepomuk::Thing>& things );
        void clear();

    Q_SIGNALS:
        void thingSelectionChanged( const QList<Nepomuk::Thing>& );
        void thingActivated( const Nepomuk::Thing& );

        /**
         * Context menu has been requested for a thing at global position pos.
         */
        void contextMenuRequested( const Nepomuk::Thing& thing, const QPoint& pos );

    protected:
        void selectionChanged( const QItemSelection& selected, const QItemSelection& deselected );
        void contextMenuEvent( QContextMenuEvent* e );

    private Q_SLOTS:
        void slotActivated( const QModelIndex& );

    private:
        class Private;
        Private* const d;
    };
}

#endif
