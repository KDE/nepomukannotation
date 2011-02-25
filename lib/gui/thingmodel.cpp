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

#include "thingmodel.h"
#include "thingmodel_p.h"

#include <Nepomuk/Types/Class>

#include <kicon.h>
#include <kcategorizedsortfilterproxymodel.h>


Q_DECLARE_METATYPE( Nepomuk::Thing )


Nepomuk::ThingModel::ThingModel( QObject* parent )
    : QAbstractListModel( parent ),
      d( new ThingModelPrivate() )
{
}


Nepomuk::ThingModel::~ThingModel()
{
    delete d;
}


QVariant Nepomuk::ThingModel::data( const QModelIndex& index, int role ) const
{
    if ( index.isValid() && index.row() < d->m_things.count() ) {
        Thing thing = thingForIndex( index );

        switch( role ) {
        case Qt::DisplayRole:
            return thing.genericLabel();

        case Qt::ToolTipRole:
            return thing.genericDescription();

        case Qt::DecorationRole:
            return KIcon( thing.genericIcon() );

        case ThingRole:
            return QVariant::fromValue( thing );

        case TypeRole:
            return thing.resourceType();

        case TypeStringRole:
        case KCategorizedSortFilterProxyModel::CategoryDisplayRole:
        case KCategorizedSortFilterProxyModel::CategorySortRole:
            return Types::Class( thing.resourceType() ).label();
        }
    }

    return QVariant();
}


int Nepomuk::ThingModel::rowCount( const QModelIndex& ) const
{
    return d->m_things.count();
}


Nepomuk::Thing Nepomuk::ThingModel::thingForIndex( const QModelIndex& index ) const
{
    if ( index.isValid() && index.row() < d->m_things.count() )
        return d->m_things[index.row()];
    else
        return Thing();
}


QList<Nepomuk::Thing> Nepomuk::ThingModel::things() const
{
    return d->m_things;
}


void Nepomuk::ThingModel::addThing( const Nepomuk::Thing& thing )
{
    beginInsertRows( QModelIndex(), d->m_things.count(), d->m_things.count() );
    d->m_things << thing;
    endInsertRows();
}


void Nepomuk::ThingModel::addThings( const QList<Nepomuk::Thing>& things )
{
    beginInsertRows( QModelIndex(), d->m_things.count(), d->m_things.count() + things.count() - 1 );
    d->m_things += things;
    endInsertRows();
}


void Nepomuk::ThingModel::setThings( const QList<Nepomuk::Thing>& things )
{
    d->m_things = things;
    reset();
}


void Nepomuk::ThingModel::clear()
{
    d->m_things.clear();
    reset();
}

#include "thingmodel.moc"
