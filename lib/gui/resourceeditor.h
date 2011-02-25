/*
   Copyright (c) 2009-2011 Sebastian Trueg <trueg@kde.org>

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

#ifndef _NEPOMUK_RESOURCE_EDITOR_H_
#define _NEPOMUK_RESOURCE_EDITOR_H_

#include <QtGui/QWidget>
#include <QtCore/QList>

#include "nepomukannotation_export.h"

class KJob;

namespace Nepomuk {

    class Resource;
    class AnnotationModel;
    class Annotation;

    class NEPOMUKANNOTATION_EXPORT ResourceEditor : public QWidget
    {
        Q_OBJECT

    public:
        ResourceEditor( QWidget* parent = 0 );
        ~ResourceEditor();

        Resource resource() const;

        bool eventFilter( QObject* obj, QEvent* event );

    Q_SIGNALS:
        /**
         * Emitted if any annotation has been added or removed or changed.
         */
        void annotationsChanged();

    public Q_SLOTS:
        /**
         * Set the resource to annotate.
         *
         * \param res The resource for which annotations are to be displayed and created.
         *        It is recommended to only use pimo things here (i.e. use Resource::pimoThing())
         * \param manualAnnotations If set no plugins will be used to create annotations
         */
        void setResource( const Nepomuk::Resource& res,
                          const QList<Nepomuk::Annotation*>& manualAnnotations = QList<Nepomuk::Annotation*>() );

        /**
         * Save the changes the user made in the resource editor.
         * If this method is not called changes are lost.
         */
        void save();

    private:
        class Private;
        Private* const d;

        Q_PRIVATE_SLOT( d, void _k_ratingChanged())
        Q_PRIVATE_SLOT( d, void _k_descriptionChanged())
        Q_PRIVATE_SLOT( d, void _k_annotationCreated(Nepomuk::Annotation* anno))
        Q_PRIVATE_SLOT( d, void _k_resourceEditRequested( const Nepomuk::Resource& res ) )
        Q_PRIVATE_SLOT( d, void _k_openNepomukBrowser() )
    };
}

#endif
