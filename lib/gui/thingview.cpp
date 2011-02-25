/*
   Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>

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

#include "thingview.h"
#include "thingmodel.h"
#include "thingitemdelegate.h"

#include <kcategorizedsortfilterproxymodel.h>
#include <kcategorydrawer.h>
#include <kdebug.h>
#include <kdialog.h>

#include <QtGui/QItemSelection>
#include <QtGui/QContextMenuEvent>


class Nepomuk::ThingView::Private
{
public:
    ThingModel* m_model;
    KCategorizedSortFilterProxyModel* m_proxyModel;

    Nepomuk::Thing thingForProxyIndex( const QModelIndex& index ) {
        return m_model->thingForIndex( m_proxyModel->mapToSource( index ) );
    }
};


Nepomuk::ThingView::ThingView( QWidget* parent )
    : KCategorizedView( parent ),
      d( new Private() )
{
    d->m_model = new ThingModel( this );
    d->m_proxyModel = new KCategorizedSortFilterProxyModel( this );
    d->m_proxyModel->setSourceModel( d->m_model );
    d->m_proxyModel->setCategorizedModel( true );
    d->m_proxyModel->sort( 0 );
    setModel( d->m_proxyModel );

    setItemDelegateForColumn( 0, new ThingItemDelegate( this ) );

    setViewMode( QListView::IconMode );
    setIconSize( QSize( 48, 48 ) );
    setCategoryDrawer( new KCategoryDrawer() );
    setResizeMode( QListView::Adjust );
    setSpacing( KDialog::spacingHint() );
    setFlow( QListView::LeftToRight );

    connect( this, SIGNAL( activated( QModelIndex ) ),
             this, SLOT( slotActivated( QModelIndex ) ) );
}


Nepomuk::ThingView::~ThingView()
{
    delete d;
}


void Nepomuk::ThingView::addThing( const Nepomuk::Thing& thing )
{
    d->m_model->addThing( thing );
}


void Nepomuk::ThingView::addThings( const QList<Nepomuk::Thing>& things )
{
    d->m_model->addThings( things );
}


void Nepomuk::ThingView::setThings( const QList<Nepomuk::Thing>& things )
{
    d->m_model->setThings( things );
}


void Nepomuk::ThingView::clear()
{
    d->m_model->clear();
}


void Nepomuk::ThingView::selectionChanged( const QItemSelection& selected, const QItemSelection& )
{
    QList<Thing> things;
    foreach( const QModelIndex& index, selected.indexes() ) {
        things << d->thingForProxyIndex( index );
    }

    kDebug() << selected;

    emit thingSelectionChanged( things );
}


void Nepomuk::ThingView::contextMenuEvent( QContextMenuEvent* e )
{
    QModelIndex index = indexAt( e->pos() );
    if ( index.isValid() )
        emit contextMenuRequested( d->thingForProxyIndex( index ), e->globalPos() );
    else
        QListView::contextMenuEvent( e );
}


void Nepomuk::ThingView::slotActivated( const QModelIndex& index )
{
    if ( index.isValid() ) {
        emit thingActivated( d->thingForProxyIndex( index ) );
    }
}

#include "thingview.moc"
