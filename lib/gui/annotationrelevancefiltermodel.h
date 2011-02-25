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

#ifndef _NEPOMUK_ANNOTATION_RELEVANCE_FILTER_MODEL_H_
#define _NEPOMUK_ANNOTATION_RELEVANCE_FILTER_MODEL_H_

#include <QtGui/QSortFilterProxyModel>
#include "nepomukannotation_export.h"

namespace Nepomuk {

    class Resource;

    class NEPOMUKANNOTATION_EXPORT AnnotationRelevanceFilterModel : public QSortFilterProxyModel
    {
        Q_OBJECT

    public:
        AnnotationRelevanceFilterModel( QObject* parent = 0 );
        ~AnnotationRelevanceFilterModel();

        double minRelevance() const;
        Resource filterResource() const;

    public Q_SLOTS:
        void setMinRelevance( double r );

        /**
         * Setting a filter resource means that all annotations that already exist
         * for this resource are not shown.
         *
         * \sa Annotation::exists
         */
        void setFilterResource( const Nepomuk::Resource& res );

    protected:
        bool filterAcceptsRow( int source_row, const QModelIndex& source_parent ) const;

    private:
        class Private;
        Private* const d;
    };
}

#endif
