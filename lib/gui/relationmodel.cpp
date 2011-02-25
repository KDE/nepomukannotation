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

#include "relationmodel.h"
#include "relationmodel_p.h"

#include <Nepomuk/Types/Class>

#include <kicon.h>
#include <Nepomuk/Variant>
#include <kcategorizedsortfilterproxymodel.h>
#include <kdebug.h>


Q_DECLARE_METATYPE( Nepomuk::RelationModel::Relation )
Q_DECLARE_METATYPE( Nepomuk::Variant )


Nepomuk::RelationModel::RelationModel( QObject* parent )
    : QAbstractListModel( parent ),
      d( new RelationModelPrivate() )
{
}


Nepomuk::RelationModel::~RelationModel()
{
    delete d;
}


QVariant Nepomuk::RelationModel::data( const QModelIndex& index, int role ) const
{
    if ( index.isValid() && index.row() < d->m_relations.count() ) {
        Relation relation = relationForIndex( index );

        switch( role ) {
        case Qt::DisplayRole:
            if ( relation.object.isResource() )
                return relation.object.toResource().pimoThing().genericLabel();
            else
                return relation.object.toString();

        case Qt::ToolTipRole:
            if ( relation.object.isResource() )
                return relation.object.toResource().pimoThing().genericDescription();

        case Qt::DecorationRole:
            if ( relation.object.isResource() )
                return KIcon( relation.object.toResource().pimoThing().genericIcon() );

        case SubjectRole:
            return QVariant::fromValue( relation.subject );

        case RelationRole:
            return QVariant::fromValue( relation );

        case ObjectRole:
            return QVariant::fromValue( relation.object );

        case TypeRole:
            if ( relation.object.isResource() )
                return relation.object.toResource().pimoThing().resourceType();

        case PropertyRole:
            return relation.property.uri();

        case KCategorizedSortFilterProxyModel::CategoryDisplayRole:
        case KCategorizedSortFilterProxyModel::CategorySortRole:
            return relation.property.label();
        }
    }

    return QVariant();
}


int Nepomuk::RelationModel::rowCount( const QModelIndex& ) const
{
    return d->m_relations.count();
}


Nepomuk::RelationModel::Relation Nepomuk::RelationModel::relationForIndex( const QModelIndex& index ) const
{
    if ( index.isValid() && index.row() < d->m_relations.count() )
        return d->m_relations[index.row()];
    else
        return Relation();
}


QList<Nepomuk::RelationModel::Relation> Nepomuk::RelationModel::relations() const
{
    return d->m_relations;
}


void Nepomuk::RelationModel::addRelation( const Nepomuk::RelationModel::Relation& relation )
{
    beginInsertRows( QModelIndex(), d->m_relations.count(), d->m_relations.count() );
    d->m_relations << relation;
    endInsertRows();
}


void Nepomuk::RelationModel::addRelations( const QList<Nepomuk::RelationModel::Relation>& relations )
{
    beginInsertRows( QModelIndex(), d->m_relations.count(), d->m_relations.count() + relations.count() - 1 );
    d->m_relations += relations;
    endInsertRows();
}


void Nepomuk::RelationModel::setRelations( const QList<Nepomuk::RelationModel::Relation>& relations )
{
    d->m_relations = relations;
    reset();
}


void Nepomuk::RelationModel::clear()
{
    d->m_relations.clear();
    reset();
}


bool Nepomuk::RelationModel::removeRows( int row, int count, const QModelIndex& parent )
{
    if ( row >= 0 && row < d->m_relations.count() &&
         count > 0 && row + count - 1 < d->m_relations.count() ) {
        beginRemoveRows( QModelIndex(), row, row+count-1 );
        while ( count-- > 0 ) {
            // remove the relation
            Relation relation = d->m_relations[row];
            if ( relation.subject.isValid() )
                relation.subject.removeProperty( relation.property.uri(), relation.object );

            // remove it from the list
            d->m_relations.removeAt( row );
        }
        endRemoveRows();
        return true;
    }
    else {
        return false;
    }
}

#include "relationmodel.moc"
