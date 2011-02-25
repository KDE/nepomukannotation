/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _SCRIBO_EXTRACTION_ANNOTATION_PLUGIN_H_
#define _SCRIBO_EXTRACTION_ANNOTATION_PLUGIN_H_

#include "annotationplugin.h"

#include <KPluginFactory>
#include <KPluginLoader>

namespace Scribo {
    class TextMatcher;
    class TextMatch;
    class Entity;
}
class KJob;

namespace Nepomuk {

    class Resource;

    class ScriboAnnotationPlugin : public AnnotationPlugin
    {
        Q_OBJECT

    public:
        ScriboAnnotationPlugin( QObject* parent, const QList<QVariant>& );

    protected:
        void doGetPossibleAnnotations( const AnnotationRequest& request );

    private Q_SLOTS:
        void slotMatchingFinished();
        void slotNewMatch( const Scribo::TextMatch& );
        void slotTextExtractionResult(KJob *job);

    private:
        /**
         * \param entity The entity to use
         * \param res The local resource to link to
         * \param score An additional weight factor to be multiplied to the match relevance
         */
        void createAnnotationForResource( const Scribo::Entity& entity, const Nepomuk::Resource& res, double score );

        Scribo::TextMatcher* m_matcher;
    };
}

#endif
