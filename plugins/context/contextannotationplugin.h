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

#ifndef _CONTEXT_ANNOTATION_PLUGIN_H_
#define _CONTEXT_ANNOTATION_PLUGIN_H_

#include "annotationplugin.h"


namespace Nepomuk {
    class ContextAnnotationPlugin : public AnnotationPlugin
    {
        Q_OBJECT

    public:
        ContextAnnotationPlugin( QObject* parent, const QVariantList& = QVariantList() );
        ~ContextAnnotationPlugin();

    protected:
        void doGetPossibleAnnotations( const AnnotationRequest& request );

    private:
    };
}

#endif
