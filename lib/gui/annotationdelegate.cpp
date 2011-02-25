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

#include "annotationdelegate.h"
#include "annotationmodel.h"
#include "annotation.h"
#include "textannotation.h"

#include <QtGui/QPainter>
#include <QtGui/QTextDocument>
#include <QtGui/QApplication>
#include <QtGui/QStyleOptionViewItemV3>
#include <QtGui/QStyle>
#include <QtGui/QMouseEvent>

#include <kicon.h>
#include <kdebug.h>

#include <Nepomuk/Resource>

Q_DECLARE_METATYPE( Nepomuk::Annotation* )

class Nepomuk::AnnotationDelegate::Private
{
public:
    Resource resource;
};


Nepomuk::AnnotationDelegate::AnnotationDelegate( QObject* parent )
    : QStyledItemDelegate( parent ),
      d( new Private )
{
}


Nepomuk::AnnotationDelegate::~AnnotationDelegate()
{
    delete d;
}


void Nepomuk::AnnotationDelegate::setResource( const Resource& res )
{
    d->resource = res;
}


namespace {
    QString buildString( Nepomuk::Annotation* anno, const QStyleOptionViewItem& option ) {
        if( anno ) {
            return QString( "<html><body><div style=\"color:%1\"><p>%2<br/><i>%3</i>" )
                .arg( option.state & QStyle::State_Selected
                      ? option.palette.color( QPalette::HighlightedText ).name()
                      : option.palette.color( QPalette::Text ).name() )
                .arg( anno->label(), anno->comment() );
        }
        else {
            return QString();
        }
    }

    QStyle* style( const QStyleOptionViewItem& option ) {
        if ( const QStyleOptionViewItemV3* v3 = qstyleoption_cast<const QStyleOptionViewItemV3*>(&option) )
            return v3->widget->style();;
        return QApplication::style();
    }

    QRect plusRect( const QStyleOptionViewItem& option ) {
        int x = ( option.rect.height()-16 )/2;
        return QRect( option.rect.right() - 16 - x, option.rect.top() + ( option.rect.height()-16 )/2, 16, 16 );
    }

    QRect minusRect( const QStyleOptionViewItem& option ) {
        int x = ( option.rect.height()-16 )/2;
        return QRect( option.rect.right() - 36 - x, option.rect.top() + ( option.rect.height()-16 )/2, 16, 16 );
    }
}

void Nepomuk::AnnotationDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    if ( option.state & QStyle::State_Selected ) {
        painter->fillRect( option.rect, option.palette.highlight() );
    }

    // one line label, one line comment (italic or something)
    // big icon before both or in the background
    // fancy buttons - and + at the right hand
    Annotation* anno = index.data( AnnotationModel::AnnotationRole ).value<Nepomuk::Annotation*>();
    double score = index.data( AnnotationModel::RelevanceRole ).toDouble();
    QIcon icon = index.data( Qt::DecorationRole ).value<QIcon>();

    QTextDocument doc;
    doc.setHtml( buildString( anno, option ) );

    if ( !icon.isNull() ) {
        int iconSize = doc.size().height();
        QPixmap pix = icon.pixmap( iconSize, iconSize );
        int iconX = option.rect.left();
        int iconY = option.rect.top() + (option.rect.height() - pix.height())/2;

        painter->save();
        painter->setOpacity( 0.3 );
        painter->drawPixmap( iconX, iconY, pix );
        painter->restore();
    }

    painter->save();
    painter->translate( option.rect.topLeft() );
    doc.drawContents( painter );
    painter->restore();

    QPixmap plus = KIcon( "list-add" ).pixmap( 16, 16 );
    QPixmap minus = KIcon( "list-remove" ).pixmap( 16, 16 );

    painter->drawPixmap( plusRect( option ), plus );
    painter->drawPixmap( minusRect( option ), minus );
}


QSize Nepomuk::AnnotationDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QFontMetrics fm( option.font );

    QTextDocument doc;
    doc.setHtml( buildString( index.data( AnnotationModel::AnnotationRole ).value<Nepomuk::Annotation*>(), option ) );

    QSize size = doc.size().toSize();
    return size + QSize( 36 + ( size.height() - 16 )/2, 0 );
}


bool Nepomuk::AnnotationDelegate::editorEvent( QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index )
{
    if ( event->type() == QEvent::MouseButtonPress ) {
        QMouseEvent* me = static_cast<QMouseEvent*>( event );
        if ( me->button() == Qt::LeftButton ) {
            if ( plusRect( option ).contains( me->pos() ) ) {
                kDebug() << "Clicked plus";
                Annotation* anno = index.data( AnnotationModel::AnnotationRole ).value<Nepomuk::Annotation*>();
                anno->create( d->resource );
                delete anno;
                return true;
            }
            else if ( minusRect( option ).contains( me->pos() ) ) {
                kDebug() << "Clicked minus";
                Annotation* anno = index.data( AnnotationModel::AnnotationRole ).value<Nepomuk::Annotation*>();
                anno->setUserRelevance( 0.0 );
                delete anno;
                return true;
            }
        }
    }

    return false;
}

#include "annotationdelegate.moc"
