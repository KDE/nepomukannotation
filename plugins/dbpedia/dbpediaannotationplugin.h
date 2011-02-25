/*
   Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _DBPEDIA_ANNOTATION_PLUGIN_H_
#define _DBPEDIA_ANNOTATION_PLUGIN_H_

#include "annotationplugin.h"

#include <QtCore/QUrl>

#include <Nepomuk/Resource>

class KJob;

namespace Soprano {
    namespace Util {
        class AsyncResult;
    }
    namespace Client {
        class SparqlModel;
    }
}

namespace Nepomuk {
    class DbpediaAnnotationPlugin : public AnnotationPlugin
    {
        Q_OBJECT

    public:
        DbpediaAnnotationPlugin( QObject* parent, const QVariantList& = QVariantList() );
        ~DbpediaAnnotationPlugin();

    protected:
        void doGetPossibleAnnotations( const AnnotationRequest& request );

    private Q_SLOTS:
        void slotQueryFinished( Soprano::Util::AsyncResult* result );

    private:
        void runQuery( const QString& text );

        Soprano::Util::AsyncResult* m_currentResult;
        Soprano::Client::SparqlModel* m_dbpediaModel;
    };
}

#endif
