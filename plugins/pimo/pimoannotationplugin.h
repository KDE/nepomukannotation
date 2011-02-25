/*
   Copyright (c) 2008-2011 Sebastian Trueg <trueg@kde.org>

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

#ifndef _NEPOMUK_PIMO_ANNOTATION_PLUGIN_H_
#define _NEPOMUK_PIMO_ANNOTATION_PLUGIN_H_

#include "annotationplugin.h"

#include <nepomuk/resource.h>
#include <nepomuk/property.h>

#include <Nepomuk/Query/Result>

#include <QtCore/QList>

namespace Nepomuk {
    namespace Types {
        class Class;
    }
    namespace Query {
        class QueryServiceClient;
    }

    class PimoAnnotationPlugin : public AnnotationPlugin
    {
        Q_OBJECT

    public:
        PimoAnnotationPlugin( QObject* parent, const QVariantList& = QVariantList() );
        ~PimoAnnotationPlugin();

        QString name() const { return "PimoAnnotationPlugin"; }

    protected:
        void doGetPossibleAnnotations( const AnnotationRequest& request );

    private Q_SLOTS:
        void slotNewEntries( const QList<Nepomuk::Query::Result>& entries );
        void slotThingSearchFinished();

    private:
        bool useType( Types::Class );

        Resource m_resource;
        QString m_filter;

        Query::QueryServiceClient* m_queryClient;
    };
}

#endif
