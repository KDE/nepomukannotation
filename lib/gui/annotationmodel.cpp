/*
   Copyright (c) 2008-2009 Sebastian Trueg <trueg@kde.org>

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

#include "annotationmodel.h"
#include "annotationmodel_p.h"
#include "annotation.h"
#include "annotation_p.h"
#include "annotationpluginwrapper.h"
#include "annotationplugin.h"
#include "annotationpluginfactory.h"

#include <kicon.h>
#include <kdebug.h>

#include <Nepomuk/Resource>

#include <QtCore/QMimeData>

Q_DECLARE_METATYPE( Nepomuk::Annotation* )


void Nepomuk::AnnotationModelPrivate::_k_annotationCreated( Nepomuk::Annotation* anno )
{
    emit q->annotationCreated( anno );
}


void Nepomuk::AnnotationModelPrivate::_k_annotationDeleted( QObject* anno )
{
    removeAnnotation( qobject_cast<Nepomuk::Annotation*>(anno) );
}


void Nepomuk::AnnotationModelPrivate::removeAnnotation( Annotation* anno )
{
    int row = annotations.indexOf( anno );
    if ( row >= 0 ) {
        q->beginRemoveRows( QModelIndex(), row, row );
        Annotation* a = annotations.takeAt( row );
        a->disconnect( q );
        q->endRemoveRows();
    }
}


Nepomuk::AnnotationModel::AnnotationModel( QObject* parent )
    : QAbstractItemModel( parent ),
      d( new AnnotationModelPrivate(this) )
{
}


Nepomuk::AnnotationModel::~AnnotationModel()
{
    delete d;
}


int Nepomuk::AnnotationModel::columnCount( const QModelIndex& ) const
{
    return 1;
}


QVariant Nepomuk::AnnotationModel::data( const QModelIndex& index, int role ) const
{
    if ( Annotation* annotation = annotationForIndex( index ) ) {
        switch( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return annotation->comment() + QString( " (%1)" ).arg( annotation->relevance() );

        case Qt::DecorationRole: {
            QIcon icon = annotation->icon();
            if ( icon.isNull() )
                icon = KIcon( "nepomuk" );
            return icon;
        }

        case AnnotationRole:
            return QVariant::fromValue( annotation );

        case RelevanceRole:
            return annotation->relevance();

        case LabelRole:
            return annotation->label();

        case CommentRole:
            return annotation->comment();
        }
    }
    return QVariant();
}


QModelIndex Nepomuk::AnnotationModel::parent( const QModelIndex& ) const
{
    return QModelIndex();
}


int Nepomuk::AnnotationModel::rowCount( const QModelIndex& parent ) const
{
    return( parent.isValid() ? 0 : d->annotations.count() );
}


QModelIndex Nepomuk::AnnotationModel::index( int row, int column, const QModelIndex& ) const
{
    if ( row < d->annotations.count() ) {
        return createIndex( row, column, d->annotations[row] );
    }
    else {
        return QModelIndex();
    }
}


Nepomuk::Annotation* Nepomuk::AnnotationModel::annotationForIndex( const QModelIndex& index ) const
{
    if ( index.internalPointer() ) {
        return static_cast<Annotation*>( index.internalPointer() );
    }
    else {
        return 0;
    }
}


Nepomuk::Annotation* Nepomuk::AnnotationModel::takeAnnotation( const QModelIndex& index )
{
    if ( index.row() < d->annotations.count() ) {
        beginRemoveRows( QModelIndex(), index.row(), index.row() );
        Annotation* a = d->annotations.takeAt( index.row() );
        a->disconnect( this );
        endRemoveRows();
        return a;
    }
    else {
        return 0;
    }
}


void Nepomuk::AnnotationModel::addNewAnnotation( Annotation* anno )
{
    kDebug() << anno->comment();

    // first check if the annotation is a duplicate, FIXME: if so, raise the relevance!
    foreach( Annotation* a, d->annotations ) {
        if ( a && a->equals( anno ) ) {
            kDebug() << "Found duplicate annotation:" << anno->comment() << "We should raise the relevance now!";
            return;
        }
    }

    beginInsertRows( QModelIndex(), d->annotations.count(), d->annotations.count() );
    d->annotations << anno;
    connect( anno, SIGNAL( finished( Nepomuk::Annotation* ) ),
             this, SLOT( _k_annotationCreated( Nepomuk::Annotation* ) ) );
    connect( anno, SIGNAL( destroyed(QObject*) ),
             this, SLOT( _k_annotationDeleted( QObject* ) ) );
    endInsertRows();
}


void Nepomuk::AnnotationModel::clear( bool deleteAnnotations )
{
    kDebug() << deleteAnnotations;
    if ( deleteAnnotations )
        qDeleteAll( d->annotations );
    d->annotations.clear();
    reset();
}


static const char* s_fakeMimeType = "internalfakestuff/x-row-number";

QMimeData* Nepomuk::AnnotationModel::mimeData( const QModelIndexList& indexes ) const
{
    QMimeData* mime = new QMimeData();
    QByteArray data;
    QDataStream s( &data, QIODevice::WriteOnly );
    foreach( const QModelIndex& index, indexes ) {
        s << index.row();
    }
    mime->setData( s_fakeMimeType, data );
    return mime;
}


QStringList Nepomuk::AnnotationModel::mimeTypes( ) const
{
    return QStringList() << s_fakeMimeType;
}


Qt::ItemFlags Nepomuk::AnnotationModel::flags( const QModelIndex& index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    if ( index.isValid() )
        flags |= Qt::ItemIsDragEnabled;
    return flags;
}


QModelIndexList Nepomuk::AnnotationModel::indexListFromMimeData( const QMimeData* mimeData ) const
{
    QByteArray data = mimeData->data( s_fakeMimeType );
    QDataStream s( data );
    QModelIndexList indices;
    while ( !s.atEnd() ) {
        int row;
        s >> row;
        indices << index( row, 0 );
    }
    return indices;
}

#include "annotationmodel.moc"
