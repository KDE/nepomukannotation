/* This file is part of the Nepomuk Project
   Copyright (c) 2010 Sebastian Trueg <trueg@kde.org>

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

#include "configwidget.h"
#include "nepomukcontextchooser.h"
#include "annotationmenu.h"
#include "classmodel.h"
#include "pimo.h"

#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QTreeView>
#include <QtGui/QListWidget>

#include <KComboBox>
#include <KDebug>
#include <KConfigGroup>

#include <Nepomuk/Types/Class>

#include <Soprano/Vocabulary/RDFS>

Q_DECLARE_METATYPE( Nepomuk::Types::Class )

static const int s_classRole = 678567;

ConfigWidget::ConfigWidget( NepomukContextChooser* applet, QWidget* parent )
    : QWidget( parent ),
      m_applet( applet )
{
    kDebug();
    setupUi( this );

    m_pimoModel = new Nepomuk::Utils::ClassModel( m_availableTypesView );
    m_pimoModel->addRootClass( Nepomuk::Vocabulary::PIMO::Thing() );
    m_pimoSortModel = new QSortFilterProxyModel( m_pimoModel );
    m_pimoSortModel->setSourceModel( m_pimoModel );
    m_pimoSortModel->setSortCaseSensitivity( Qt::CaseInsensitive );
    m_pimoSortModel->setDynamicSortFilter( true );

    m_availableTypesView->setModel( m_pimoSortModel );
    m_availableTypesView->setSortingEnabled( true );
    m_availableTypesView->sortByColumn( 0, Qt::AscendingOrder );

    m_typeGroupCombo->addItem( "PIMO Types", QVariant( Nepomuk::Vocabulary::PIMO::Thing() ) );
    m_typeGroupCombo->addItem( "All Types", QVariant( Soprano::Vocabulary::RDFS::Resource() ) );

    connect( m_availableTypesView->selectionModel(), SIGNAL( selectionChanged(QItemSelection, QItemSelection) ),
             this, SLOT( slotAvailableTypeSelectionChanged(QItemSelection, QItemSelection) ) );
    connect( m_selectedTypesView->selectionModel(), SIGNAL( selectionChanged(QItemSelection, QItemSelection) ),
             this, SLOT( slotSelectedTypeSelectionChanged(QItemSelection, QItemSelection) ) );
    connect( m_typeGroupCombo, SIGNAL( activated(int) ),
             this, SLOT( slotTypeGroupChanged(int) ) );
    connect( m_addButton, SIGNAL( clicked() ), this, SLOT( slotAddButtonClicked() ) );
    connect( m_removeButton, SIGNAL( clicked() ), this, SLOT( slotRemoveButtonClicked() ) );
    connect( m_sortUpButton, SIGNAL( clicked() ), this, SLOT( slotSortUpButtonClicked() ) );
    connect( m_sortDownButton, SIGNAL( clicked() ), this, SLOT( slotSortDownButtonClicked() ) );

    slotAvailableTypeSelectionChanged( QItemSelection(), QItemSelection() );
    slotSelectedTypeSelectionChanged( QItemSelection(), QItemSelection() );

    Q_FOREACH( const Nepomuk::Types::Class& type, applet->annotationMenu()->supportedTypes() ) {
        addSupportedType( type );
    }
}


ConfigWidget::~ConfigWidget()
{
    kDebug();
}


QList<Nepomuk::Types::Class> ConfigWidget::selectedTypes() const
{
    QList<Nepomuk::Types::Class> types;
    for ( int i = 0; i < m_selectedTypesView->count(); ++i ) {
        types << m_selectedTypesView->item( i )->data( s_classRole ).value<Nepomuk::Types::Class>();
    }
    return types;
}


// static
QList<Nepomuk::Types::Class> ConfigWidget::readTypesFromConfig( const KConfigGroup& config )
{
    QStringList typeStrings = config.readEntry( "selected types", QStringList() );
    kDebug() << typeStrings;
    QList<Nepomuk::Types::Class> types;
    Q_FOREACH( const QString& typeString, typeStrings ) {
        types << KUrl( typeString );
    }
    return types;
}


// static
void ConfigWidget::writeTypesToConfig( KConfigGroup config, const QList<Nepomuk::Types::Class>& types )
{
    QStringList typeStrings;
    Q_FOREACH( const Nepomuk::Types::Class& type, types ) {
        typeStrings << KUrl( type.uri() ).url();
    }
    config.writeEntry( "selected types", typeStrings );
}


void ConfigWidget::updateAnnotationMenuConfig()
{
    m_applet->annotationMenu()->setSupportedTypes( selectedTypes() );
    writeTypesToConfig( m_applet->config(), selectedTypes() );
}


void ConfigWidget::slotAvailableTypeSelectionChanged( const QItemSelection& selected, const QItemSelection& )
{
    m_addButton->setEnabled( !selected.isEmpty() );
}


void ConfigWidget::slotSelectedTypeSelectionChanged( const QItemSelection& selected, const QItemSelection& )
{
    if ( !selected.isEmpty() ) {
        m_removeButton->setEnabled(true );
        QModelIndex index = selected.indexes().first();
        m_sortUpButton->setEnabled( index.row() != 0 );
        m_sortDownButton->setEnabled( index.row() != m_selectedTypesView->count()-1 );
    }
    else {
        m_removeButton->setEnabled( false );
        m_sortUpButton->setEnabled( false );
        m_sortDownButton->setEnabled( false );
    }
}


void ConfigWidget::slotTypeGroupChanged( int index )
{
    m_pimoModel->setRootClass( m_typeGroupCombo->itemData( index ).toUrl() );
    slotAvailableTypeSelectionChanged( QItemSelection(), QItemSelection() );
}


void ConfigWidget::slotAddButtonClicked()
{
    QModelIndex index = m_availableTypesView->selectionModel()->selectedIndexes().first();
    Nepomuk::Types::Class type
        = index.data( Nepomuk::Utils::ClassModel::TypeRole ).value<Nepomuk::Types::Class>();
    addSupportedType( type );
}


void ConfigWidget::slotRemoveButtonClicked()
{
    delete m_selectedTypesView->currentItem();
}


void ConfigWidget::addSupportedType( const Nepomuk::Types::Class& type )
{
    QListWidgetItem* item = new QListWidgetItem( m_selectedTypesView );
    item->setText( type.label() );
    item->setData( s_classRole, QVariant::fromValue( type ) );
}


void ConfigWidget::slotSortUpButtonClicked()
{
    int row = m_selectedTypesView->currentRow();
    QListWidgetItem* item = m_selectedTypesView->takeItem( row );
    m_selectedTypesView->insertItem( row-1, item );
    m_selectedTypesView->setCurrentItem( item );
}


void ConfigWidget::slotSortDownButtonClicked()
{
    int row = m_selectedTypesView->currentRow();
    QListWidgetItem* item = m_selectedTypesView->takeItem( row );
    m_selectedTypesView->insertItem( row+1, item );
    m_selectedTypesView->setCurrentItem( item );
}

#include "configwidget.moc"
