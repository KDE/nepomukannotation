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

#ifndef _NEPOMUK_ANNOTATION_MODEL_H_
#define _NEPOMUK_ANNOTATION_MODEL_H_

#include <QtCore/QAbstractItemModel>

#include "nepomukannotation_export.h"

#include <nepomuk/resource.h>

namespace Nepomuk {

    class Annotation;
    class AnnotationModelPrivate;

    class NEPOMUKANNOTATION_EXPORT AnnotationModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        AnnotationModel( QObject* );
        virtual ~AnnotationModel();

        int columnCount( const QModelIndex& parent = QModelIndex() ) const;
        QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
        QModelIndex parent( const QModelIndex& index ) const;
        int rowCount( const QModelIndex& parent = QModelIndex() ) const;
        QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex() ) const;

        QMimeData* mimeData( const QModelIndexList& indexes ) const;
        QStringList mimeTypes( ) const;
        Qt::ItemFlags flags( const QModelIndex& index ) const;

        Annotation* annotationForIndex( const QModelIndex& index ) const;

        /**
         * Removes the Annotation from the model and returns it.
         * This is useful for execution of the Annotation.
         *
         * \deprecated Since Annotations delete itself after creation
         * they are automatically removed from the model
         */
        Annotation* takeAnnotation( const QModelIndex& index );

        enum Roles {
            AnnotationRole = 7969856,
            LabelRole = 879790843,
            CommentRole = 55476238,
            RelevanceRole = 96778976
        };

        QModelIndexList indexListFromMimeData( const QMimeData* mimeData ) const;

    Q_SIGNALS:
        /**
         * This signal is simply forwared from Annotation::finished.
         * It is provided for convinience. Be aware that it will not
         * be emitted if the annotation is taken via takeAnnotation
         * before creation.
         */
        void annotationCreated( Nepomuk::Annotation* );

    public Q_SLOTS:
        void addNewAnnotation( Nepomuk::Annotation* );
        void clear( bool deleteAnnotations = false );

    private:
        AnnotationModelPrivate* const d;
        friend class AnnotationModelPrivate;

        Q_PRIVATE_SLOT( d, void _k_annotationCreated( Nepomuk::Annotation* ) )
        Q_PRIVATE_SLOT( d, void _k_annotationDeleted( QObject* ) )
    };
}

#endif
