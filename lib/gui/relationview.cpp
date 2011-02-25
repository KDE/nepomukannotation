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

#include "relationview.h"
#include "relationview_p.h"
#include "relationmodel.h"
#include "relationitemdelegate.h"

#include <kcategorizedsortfilterproxymodel.h>
#include <kcategorydrawer.h>
#include <kdebug.h>
#include <kdialog.h>

#include <QtGui/QItemSelection>
#include <QtGui/QContextMenuEvent>


Nepomuk::RelationView::RelationView( QWidget* parent )
    : KCategorizedView( parent ),
      d( new RelationViewPrivate() )
{
    d->m_model = new RelationModel( this );
    d->m_proxyModel = new KCategorizedSortFilterProxyModel( this );
    d->m_proxyModel->setSourceModel( d->m_model );
    d->m_proxyModel->setCategorizedModel( true );
    d->m_proxyModel->sort( 0 );
    setModel( d->m_proxyModel );

    RelationItemDelegate* delegate = new RelationItemDelegate( this );
    setItemDelegateForColumn( 0, delegate );

    setViewMode( QListView::IconMode );
    setIconSize( QSize( 48, 48 ) );
    setCategoryDrawer( new KCategoryDrawer() );
    setResizeMode( QListView::Adjust );
    setSpacing( KDialog::spacingHint() );
    setFlow( QListView::LeftToRight );

    connect( this, SIGNAL( activated( QModelIndex ) ),
             this, SLOT( slotActivated( QModelIndex ) ) );
    connect( delegate, SIGNAL( resourceEditRequested( Nepomuk::Resource ) ),
             this, SIGNAL( resourceEditRequested( Nepomuk::Resource ) ) );
}


Nepomuk::RelationView::~RelationView()
{
    delete d;
}


void Nepomuk::RelationView::addRelation( const Nepomuk::RelationModel::Relation& relation )
{
    d->m_model->addRelation( relation );
}


void Nepomuk::RelationView::addRelations( const QList<Nepomuk::RelationModel::Relation>& relations )
{
    d->m_model->addRelations( relations );
}


void Nepomuk::RelationView::setRelations( const QList<Nepomuk::RelationModel::Relation>& relations )
{
    d->m_model->setRelations( relations );
}


void Nepomuk::RelationView::clear()
{
    d->m_model->clear();
}


void Nepomuk::RelationView::selectionChanged( const QItemSelection& selected, const QItemSelection& )
{
    QList<RelationModel::Relation> relations;
    foreach( const QModelIndex& index, selected.indexes() ) {
        relations << d->relationForProxyIndex( index );
    }

    kDebug() << selected;

    emit relationSelectionChanged( relations );
}


void Nepomuk::RelationView::contextMenuEvent( QContextMenuEvent* e )
{
    QModelIndex index = indexAt( e->pos() );
    if ( index.isValid() )
        emit contextMenuRequested( d->relationForProxyIndex( index ), e->globalPos() );
    else
        QListView::contextMenuEvent( e );
}


void Nepomuk::RelationView::slotActivated( const QModelIndex& index )
{
    if ( index.isValid() ) {
        emit relationActivated( d->relationForProxyIndex( index ) );
    }
}

#include "relationview.moc"
