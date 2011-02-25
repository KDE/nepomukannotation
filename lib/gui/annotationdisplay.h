/*
   Copyright (c) 2008 Sebastian Trueg <trueg@kde.org>

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

#ifndef _NEPOMUK_ANNOTATION_DISPLAY_H_
#define _NEPOMUK_ANNOTATION_DISPLAY_H_

#include <QtGui/QWidget>
#include <QtCore/QUrl>
#include <QtCore/QList>

#include "nepomukannotation_export.h"

#include <Nepomuk/Resource>
#include <Nepomuk/Types/Property>

namespace Nepomuk {
    class NEPOMUKANNOTATION_EXPORT AnnotationDisplay : public QWidget
    {
        Q_OBJECT

    public:
        AnnotationDisplay( QWidget* parent = 0 );
        ~AnnotationDisplay();

        /**
         * \return \p true if navigation is enabled.
         *
         * \sa setNavigationEnabled
         */
        bool isNavigationEnabled() const;
        bool isPropertyRemovalEnabled() const;

        QList<Nepomuk::Types::Property> hiddenProperties() const;

        Resource resource() const;

    public Q_SLOTS:
        void setResource( const Nepomuk::Resource& resource );

        /**
         * Enable or disable navigation. If navigation is enabled
         * the displayedResourceChanged signal will be emitted
         * and the display will allow to navigate through the resouce
         * graph. If navigation is disabled the resource can only be
         * changed programmatically with setResource and links are
         * executed via the KDE system, i.e. opened in Konqueror
         * or KMail.
         *
         * \sa isNavigationEnabled
         */
        void setNavigationEnabled( bool );

        void setPropertyRemovalEnabled( bool );

        void setHiddenProperties( const QList<Nepomuk::Types::Property>& properties );
        void addHiddenProperty( const Nepomuk::Types::Property& property );

    Q_SIGNALS:
        /**
         * Emitted when the user navigates to a different resource.
         *
         * \sa setNavigationEnabled
         */
        void displayedResourceChanged( const Nepomuk::Resource& );

    private:
        class Private;
        Private* const d;
    };
}

#endif
