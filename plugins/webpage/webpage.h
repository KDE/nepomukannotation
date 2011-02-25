/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
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

#ifndef ANNOTATIONPLUGIN_WEBPAGE_H
#define ANNOTATIONPLUGIN_WEBPAGE_H

#include "annotationplugin.h"

#include <soprano/soprano.h>
#include <KPluginFactory>
#include <KPluginLoader>

class KJob;
namespace KIO {
    class MimetypeJob;
}

namespace Nepomuk {
    class WebpageAnnotationPlugin : public AnnotationPlugin
    {
        Q_OBJECT

    public:
        WebpageAnnotationPlugin(QObject* parent, const QList<QVariant>&);
        ~WebpageAnnotationPlugin();

    private Q_SLOTS:
        void slotMimetypeResult( KJob* job );

    private:
        void doGetPossibleAnnotations( const AnnotationRequest& request );

        KIO::MimetypeJob* m_lastJob;
    };

} // End namespace Nepomuk

#endif
