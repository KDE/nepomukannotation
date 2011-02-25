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

#ifndef _NEPOMUK_THING_MODEL_H_
#define _NEPOMUK_THING_MODEL_H_

#include <QtCore/QAbstractListModel>

#include "nepomukannotation_export.h"

#include <Nepomuk/Thing>

namespace Nepomuk {

    class ThingModelPrivate;

    class NEPOMUKANNOTATION_EXPORT ThingModel : public QAbstractListModel
    {
        Q_OBJECT

    public:
        ThingModel( QObject* );
        virtual ~ThingModel();

        QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
        int rowCount( const QModelIndex& parent = QModelIndex() ) const;

        Thing thingForIndex( const QModelIndex& index ) const;

        QList<Thing> things() const;

        enum Roles {
            /**
             * Will return the Thing itself
             */
            ThingRole = 9768891,

            /**
             * Will return the main type of the thing
             * as a QUrl.
             *
             * \sa Nepomuk::Resource::resourceType
             */
            TypeRole = 587233,

            /**
             * Will return the label or the type
             * for categorization.
             */
            TypeStringRole = 768975
        };

    public Q_SLOTS:
        void addThing( const Nepomuk::Thing& thing );
        void addThings( const QList<Nepomuk::Thing>& things );
        void setThings( const QList<Nepomuk::Thing>& things );
        void clear();

    private:
        ThingModelPrivate* const d;
        friend class ThingModelPrivate;
    };
}

#endif
