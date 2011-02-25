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

#include "annotationdisplay.h"
#include "annotationdisplaybrowser.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QToolBar>

#include <kstandardaction.h>
#include <kaction.h>


class Nepomuk::AnnotationDisplay::Private
{
public:
    AnnotationDisplayBrowser* m_browser;
    QToolBar* m_toolbar;
    KAction* m_backAction;
    KAction* m_forwardAction;

    void setEnabledToolBar( bool );

    AnnotationDisplay* q;
};


void Nepomuk::AnnotationDisplay::Private::setEnabledToolBar( bool enabled )
{
    if ( enabled ) {
        if ( !m_toolbar ) {
            m_toolbar = new QToolBar( q );
            static_cast<QVBoxLayout*>( q->layout() )->insertWidget( 0, m_toolbar );
            m_backAction = KStandardAction::back( m_browser, SLOT( backward() ), m_toolbar );
            m_forwardAction = KStandardAction::forward( m_browser, SLOT( forward() ), m_toolbar );
            m_toolbar->addAction( m_backAction );
            m_toolbar->addAction( m_forwardAction );
            m_backAction->setEnabled( false );
            m_forwardAction->setEnabled( false );
            connect( m_browser, SIGNAL( backwardAvailable(bool) ), m_backAction, SLOT( setEnabled( bool ) ) );
            connect( m_browser, SIGNAL( forwardAvailable(bool) ), m_forwardAction, SLOT( setEnabled( bool ) ) );
        }
    }
    if ( m_toolbar ) {
        m_toolbar->setVisible( enabled );
    }
}


Nepomuk::AnnotationDisplay::AnnotationDisplay( QWidget* parent )
    : QWidget( parent ),
      d( new Private() )
{
    d->m_browser = new AnnotationDisplayBrowser( this );
    d->m_toolbar = 0;
    d->q = this;
    QVBoxLayout* lay = new QVBoxLayout( this );
    lay->setMargin( 0 );
    lay->addWidget( d->m_browser );
    connect( d->m_browser, SIGNAL( displayedResourceChanged( Nepomuk::Resource ) ),
             this, SIGNAL( displayedResourceChanged( Nepomuk::Resource ) ) );
}


Nepomuk::AnnotationDisplay::~AnnotationDisplay()
{
    delete d;
}


void Nepomuk::AnnotationDisplay::setResource( const Resource& resource )
{
    d->m_browser->setResource( resource );
}


Nepomuk::Resource Nepomuk::AnnotationDisplay::resource() const
{
    return d->m_browser->resource();
}


void Nepomuk::AnnotationDisplay::setNavigationEnabled( bool enabled )
{
    d->m_browser->setNavigationEnabled( enabled );
    d->setEnabledToolBar( enabled );
}


void Nepomuk::AnnotationDisplay::setPropertyRemovalEnabled( bool enabled )
{
    d->m_browser->setPropertyRemovalEnabled( enabled );
}


bool Nepomuk::AnnotationDisplay::isNavigationEnabled() const
{
    return d->m_browser->isNavigationEnabled();
}


bool Nepomuk::AnnotationDisplay::isPropertyRemovalEnabled() const
{
    return d->m_browser->isPropertyRemovalEnabled();
}


void Nepomuk::AnnotationDisplay::setHiddenProperties( const QList<Nepomuk::Types::Property>& properties )
{
    d->m_browser->setHiddenProperties( properties );
}


void Nepomuk::AnnotationDisplay::addHiddenProperty( const Nepomuk::Types::Property& property )
{
    d->m_browser->addHiddenProperty( property );
}


QList<Nepomuk::Types::Property> Nepomuk::AnnotationDisplay::hiddenProperties() const
{
    return d->m_browser->hiddenProperties();
}

#include "annotationdisplay.moc"
