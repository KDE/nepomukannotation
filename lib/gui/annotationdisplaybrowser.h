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

#ifndef _NEPOMUK_ANNOTATION_DISPLAY_BROWSER_H_
#define _NEPOMUK_ANNOTATION_DISPLAY_BROWSER_H_

#include <ktextbrowser.h>
#include <QtCore/QList>

#include <Nepomuk/Resource>
#include <Nepomuk/Types/Property>

namespace Nepomuk {
    class AnnotationDisplayBrowser : public KTextBrowser
    {
        Q_OBJECT

    public:
        AnnotationDisplayBrowser( QWidget* parent );
        ~AnnotationDisplayBrowser();

        bool isNavigationEnabled() const;
        bool isPropertyRemovalEnabled() const;

        Resource resource() const;

        QList<Nepomuk::Types::Property> hiddenProperties() const;

    public Q_SLOTS:
        void setResource( const Nepomuk::Resource& resource );
        void setNavigationEnabled( bool );
        void setPropertyRemovalEnabled( bool );
        void setHiddenProperties( const QList<Nepomuk::Types::Property>& properties );
        void addHiddenProperty( const Nepomuk::Types::Property& property );
        void backward();
        void forward();

    Q_SIGNALS:
        void displayedResourceChanged( const Nepomuk::Resource& );

    protected:
        void setSource( const QUrl& url );

    private:
        class Private;
        Private* const d;

        Q_PRIVATE_SLOT( d, void _k_slotDataChanged() )
    };
}

#endif
