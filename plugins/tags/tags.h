/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
 * Copyright (c) 2008-2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef ANNOTATIONPLUGIN_TAGS_H
#define ANNOTATIONPLUGIN_TAGS_H

#include "annotationplugin.h"

#include <KPluginFactory>
#include <KPluginLoader>

namespace Nepomuk {

    class AnnotationResource;
    class AnnotationResult;

    class TagsAnnotationPlugin : public AnnotationPlugin
    {
        Q_OBJECT

    public:
        TagsAnnotationPlugin(QObject* parent, const QList<QVariant>&);

    protected:
        void doGetPossibleAnnotations( const AnnotationRequest& request );
    };

} // End namespace Nepomuk

#endif
