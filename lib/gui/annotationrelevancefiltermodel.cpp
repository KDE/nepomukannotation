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

#include "annotationrelevancefiltermodel.h"
#include "annotationmodel.h"
#include "annotation.h"

#include <Nepomuk/Resource>


Q_DECLARE_METATYPE( Nepomuk::Annotation* )

class Nepomuk::AnnotationRelevanceFilterModel::Private
{
public:
    double minRelevance;
    Resource filterResource;
};


Nepomuk::AnnotationRelevanceFilterModel::AnnotationRelevanceFilterModel( QObject* parent )
    : QSortFilterProxyModel( parent ),
      d( new Private() )
{
    d->minRelevance = 0.0;
}


Nepomuk::AnnotationRelevanceFilterModel::~AnnotationRelevanceFilterModel()
{
    delete d;
}


double Nepomuk::AnnotationRelevanceFilterModel::minRelevance() const
{
    return d->minRelevance;
}


Nepomuk::Resource Nepomuk::AnnotationRelevanceFilterModel::filterResource() const
{
    return d->filterResource;
}


void Nepomuk::AnnotationRelevanceFilterModel::setMinRelevance( double r )
{
    d->minRelevance = r;
}


void Nepomuk::AnnotationRelevanceFilterModel::setFilterResource( const Nepomuk::Resource& res )
{
    d->filterResource = res;
    invalidateFilter();
}


bool Nepomuk::AnnotationRelevanceFilterModel::filterAcceptsRow( int source_row, const QModelIndex& source_parent ) const
{
    double relevance = sourceModel()->index( source_row, 0, source_parent ).data( AnnotationModel::RelevanceRole ).toDouble();
    if ( relevance < d->minRelevance ) {
        return false;
    }
    else if ( d->filterResource.isValid() ) {
        if ( Annotation* a = sourceModel()->index( source_row, 0, source_parent ).data( AnnotationModel::AnnotationRole ).value<Nepomuk::Annotation*>() ) {
            return !a->exists( d->filterResource );
        }
    }

    return true;
}

#include "annotationrelevancefiltermodel.moc"
