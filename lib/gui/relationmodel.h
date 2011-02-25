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
#include <Nepomuk/Variant>
#include <Nepomuk/Types/Property>

namespace Nepomuk {

    class RelationModelPrivate;

    class NEPOMUKANNOTATION_EXPORT RelationModel : public QAbstractListModel
    {
        Q_OBJECT

    public:
        RelationModel( QObject* );
        virtual ~RelationModel();

        /**
         * Represents a relation between two resources. \m resource may
         * be undefined for cases in which the model should contain
         * relations that are to be created and do not exist yet or
         * in case the relations represent multiple resources.
         */
        struct Relation {
            Relation( const Nepomuk::Resource& r = Resource(),
                      const Nepomuk::Types::Property& p = Types::Property(),
                      const Nepomuk::Variant& t = Resource() )
                : subject( r ),
                  property( p ),
                  object( t ) {}

            Nepomuk::Resource subject;
            Nepomuk::Types::Property property;
            Nepomuk::Variant object;
        };

        QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
        int rowCount( const QModelIndex& parent = QModelIndex() ) const;

        Relation relationForIndex( const QModelIndex& index ) const;

        QList<Relation> relations() const;

        enum Roles {
            RelationRole = 687658,

            SubjectRole = 68768923,

            /**
             * Will return the Thing itself
             */
            ObjectRole = 9768891,

            /**
             * Will return the main type of the thing
             * as a QUrl.
             *
             * \sa Nepomuk::Resource::resourceType
             */
            TypeRole = 587233,

            /**
             * Will return the property as a QUrl.
             */
            PropertyRole = 768975
        };

        /**
         * Will remove the row from the model and actually delete the property in the resource (if the
         * resource is set)
         */
        bool removeRows( int row, int count, const QModelIndex& parent = QModelIndex() );

    public Q_SLOTS:
        void addRelation( const Nepomuk::RelationModel::Relation& rel );
        void addRelations( const QList<Nepomuk::RelationModel::Relation>& rels );
        void setRelations( const QList<Nepomuk::RelationModel::Relation>& rels );
        void clear();

    private:
        RelationModelPrivate* const d;
        friend class RelationModelPrivate;
    };
}

#endif
