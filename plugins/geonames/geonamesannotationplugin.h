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

#ifndef _GEO_NAMES_ANNOTATION_PLUGIN_H_
#define _GEO_NAMES_ANNOTATION_PLUGIN_H_

#include "annotationplugin.h"

#include <QtCore/QUrl>

#include <Nepomuk/Resource>

class KJob;

namespace Nepomuk {

    class GraphRetriever;

    class GeoNamesAnnotationPlugin : public AnnotationPlugin
    {
        Q_OBJECT

    public:
        GeoNamesAnnotationPlugin( QObject* parent, const QVariantList& = QVariantList() );
        ~GeoNamesAnnotationPlugin();

    protected:
        void doGetPossibleAnnotations( const AnnotationRequest& request );

    private Q_SLOTS:
        void slotGeoNamesQueryResult( KJob* );

    private:
        GraphRetriever* m_lastRdfJob;
    };
}

#endif
