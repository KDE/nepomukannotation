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

#include "relationitemdelegate.h"
#include "relationmodel.h"
#include "relationview.h"

#include <Nepomuk/Thing>
#include <Nepomuk/Types/Class>

#include <QtGui/QPushButton>
#include <QtGui/QTextDocument>
#include <QtGui/QPainter>
#include <QtGui/QToolButton>

#include <kicon.h>
#include <kdebug.h>


Q_DECLARE_METATYPE( Nepomuk::Variant )
Q_DECLARE_METATYPE( Nepomuk::RelationModel::Relation )

Nepomuk::RelationItemDelegate::RelationItemDelegate( RelationView* parent )
    : KWidgetItemDelegate( parent, parent )
{
}


Nepomuk::RelationItemDelegate::~RelationItemDelegate()
{
}


QList<QWidget*> Nepomuk::RelationItemDelegate::createItemWidgets() const
{
    // the remove button
    QToolButton* deleteButton = new QToolButton;
    deleteButton->setIcon( KIcon( "list-remove" ) );
    deleteButton->setToolTip( i18n( "Remove relation" ) );
    deleteButton->setAutoRaise( true );
    connect( deleteButton, SIGNAL( clicked() ), this, SLOT( slotDeleteClicked() ) );

    // the edit button
    QToolButton* editButton = new QToolButton;
    editButton->setIcon( KIcon( "document-edit" ) );
    editButton->setToolTip( i18n( "Edit" ) );
    editButton->setAutoRaise( true );
    connect( editButton, SIGNAL( clicked() ), this, SLOT( slotEditClicked() ) );

    m_buttonSize = deleteButton->sizeHint();

    setBlockedEventTypes( deleteButton,
                          QList<QEvent::Type>()
                          << QEvent::MouseButtonPress
                          << QEvent::MouseButtonRelease
                          << QEvent::MouseButtonDblClick
                          << QEvent::KeyPress
                          << QEvent::KeyRelease );
    setBlockedEventTypes( editButton,
                          QList<QEvent::Type>()
                          << QEvent::MouseButtonPress
                          << QEvent::MouseButtonRelease
                          << QEvent::MouseButtonDblClick
                          << QEvent::KeyPress
                          << QEvent::KeyRelease );

    return QList<QWidget*>() << deleteButton << editButton;
}


void Nepomuk::RelationItemDelegate::updateItemWidgets( const QList<QWidget*> widgets,
                                                       const QStyleOptionViewItem& option,
                                                       const QPersistentModelIndex& index ) const
{
    QToolButton* deleteButton = static_cast<QToolButton*>( widgets[0] );
    QSize buttonSize = deleteButton->sizeHint();
    deleteButton->resize( buttonSize );
    deleteButton->move( option.rect.right() - 10 - ( 2*buttonSize.width() ),
                        ( option.rect.height() - buttonSize.height() )/2 );

    QToolButton* editButton = static_cast<QToolButton*>( widgets[1] );
    buttonSize = editButton->sizeHint();
    editButton->resize( buttonSize );
    editButton->move( option.rect.right() - 10 - buttonSize.width(),
                      ( option.rect.height() - buttonSize.height() )/2 );

    deleteButton->setVisible( index.isValid() &&
                              index.data( RelationModel::SubjectRole ).value<Nepomuk::Resource>().isValid() );
    editButton->setVisible( index.isValid() &&
                            ( index.data( RelationModel::ObjectRole ).value<Nepomuk::Variant>().isResource() ||
                              index.data( RelationModel::ObjectRole ).value<Nepomuk::Variant>().isUrl() ) );
}


namespace {
    QString relationText( const Nepomuk::RelationModel::Relation& relation )
    {
        if ( relation.object.isResource() )
            return QString( "<p>%1 <i>%2</i>" )
                .arg( relation.object.toResource().genericLabel() )
                .arg( Nepomuk::Types::Class( relation.object.toResource().resourceType() ).label() );
        else
            return relation.object.toString();
    }
}

void Nepomuk::RelationItemDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    RelationModel::Relation relation = index.data( RelationModel::RelationRole ).value<Nepomuk::RelationModel::Relation>();
    QTextDocument doc;
    doc.setHtml( QString("<html><body><div style=\"color:%1\">%2</div></body></html>")
                     .arg( option.state & QStyle::State_Selected
                           ? option.palette.color( QPalette::HighlightedText ).name()
                           : option.palette.color( QPalette::Text ).name() )
                 .arg( relationText( relation ) ) );

    painter->save();

    if ( option.state & QStyle::State_Selected ) {
        painter->fillRect( option.rect, option.palette.highlight() );
    }

    painter->translate( option.rect.topLeft() );
    doc.drawContents( painter );

    painter->restore();
}


QSize Nepomuk::RelationItemDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    Q_UNUSED( option );

    RelationModel::Relation relation = index.data( RelationModel::RelationRole ).value<Nepomuk::RelationModel::Relation>();
    QTextDocument doc;
    doc.setHtml( relationText( relation ) );
    return doc.size().toSize() + QSize( 10 + ( 2*m_buttonSize.width() ), 0 );
}


void Nepomuk::RelationItemDelegate::slotDeleteClicked()
{
    itemView()->model()->removeRow( focusedIndex().row(), QModelIndex() );
}


void Nepomuk::RelationItemDelegate::slotEditClicked()
{
    Resource res = focusedIndex().data( RelationModel::ObjectRole ).value<Nepomuk::Variant>().toResource();
    kDebug() << res.resourceUri();
    if ( res.isValid() )
        emit resourceEditRequested( res );
}

#include "relationitemdelegate.moc"
