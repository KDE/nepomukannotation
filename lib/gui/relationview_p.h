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

#ifndef _NEPOMUK_RELATION_VIEW_P_H_
#define _NEPOMUK_RELATION_VIEW_P_H_

#include "relationmodel.h"

#include <kcategorizedsortfilterproxymodel.h>

namespace Nepomuk {
    class RelationViewPrivate
    {
    public:
        RelationModel* m_model;
        KCategorizedSortFilterProxyModel* m_proxyModel;

        RelationModel::Relation relationForProxyIndex( const QModelIndex& index ) {
            return m_model->relationForIndex( m_proxyModel->mapToSource( index ) );
        }
    };
}

#endif
