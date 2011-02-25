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

#include "labeleditwidget.h"

#include <klineedit.h>
#include <ksqueezedtextlabel.h>
#include <kicon.h>

#include <QtGui/QLabel>
#include <QtGui/QToolButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QStackedLayout>
#include <QtGui/QKeyEvent>


class LabelEditWidget::Private
{
public:
    LabelEditWidget* q;

    QString m_text;

    KSqueezedTextLabel* m_label;
    KLineEdit* m_lineEdit;
    QToolButton* m_button;
    QWidget* m_labelContainer;
    QStackedLayout* m_stack;

    void toggleEditor( bool enabled, bool saveText );

    void _k_buttonClicked();
    void _k_editingFinished();
    void _k_textChanged( const QString& text );
};


void LabelEditWidget::Private::toggleEditor( bool enabled, bool saveText )
{
    if ( enabled ) {
        if ( m_stack->currentWidget() != m_lineEdit ) {
            if ( saveText )
                m_lineEdit->setText( m_text );
            m_stack->setCurrentWidget( m_lineEdit );
            m_lineEdit->setFocus();
        }
    }
    else {
        if ( m_stack->currentWidget() == m_lineEdit ) {
            if ( saveText ) {
                m_text = m_lineEdit->text();
                m_label->setText( m_text );
            }
            m_stack->setCurrentWidget( m_labelContainer );
        }
    }
}


void LabelEditWidget::Private::_k_buttonClicked()
{
    toggleEditor( true, true );
}


void LabelEditWidget::Private::_k_editingFinished()
{
    if ( m_stack->currentWidget() == m_lineEdit ) {
        toggleEditor( false, false );
        emit q->editingFinished( false );
    }
}


void LabelEditWidget::Private::_k_textChanged( const QString& text )
{
    // we do not want to emit the text changed if we switch to edit mode
    if ( m_stack->currentWidget() == m_lineEdit )
        emit q->textChanged( text );
}


LabelEditWidget::LabelEditWidget( QWidget* parent )
    : QWidget( parent ),
      d( new Private() )
{
    d->q = this;

    d->m_labelContainer = new QWidget( this );
    d->m_label = new KSqueezedTextLabel( d->m_labelContainer );
    d->m_label->installEventFilter( this );
    d->m_button = new QToolButton( d->m_labelContainer );
    d->m_button->setIcon( KIcon( "edit-rename" ) );
    d->m_button->setAutoRaise( true );

    QHBoxLayout* lay = new QHBoxLayout( d->m_labelContainer );
    lay->setMargin( 0 );
    lay->addWidget( d->m_label );
    lay->addWidget( d->m_button );

    d->m_lineEdit = new KLineEdit( this );
    d->m_lineEdit->installEventFilter( this );

    d->m_stack = new QStackedLayout( this );
    d->m_stack->setMargin( 0 );
    d->m_stack->addWidget( d->m_labelContainer );
    d->m_stack->addWidget( d->m_lineEdit );

    connect( d->m_lineEdit, SIGNAL( textEdited(QString) ),
             this, SIGNAL( textEdited(QString) ) );
    connect( d->m_lineEdit, SIGNAL( textChanged(QString) ),
             this, SLOT( _k_textChanged(QString) ) );
    connect( d->m_lineEdit, SIGNAL( editingFinished() ),
             this, SLOT( _k_editingFinished() ) );
    connect( d->m_button, SIGNAL( clicked() ),
             this, SLOT( _k_buttonClicked() ) );

    d->m_stack->setCurrentWidget( d->m_labelContainer );
}


LabelEditWidget::~LabelEditWidget()
{
    delete d;
}


QString LabelEditWidget::text() const
{
    if ( d->m_stack->currentWidget() == d->m_labelContainer )
        return d->m_text;
    else
        return d->m_lineEdit->text();
}


void LabelEditWidget::setText( const QString& text )
{
    d->toggleEditor( false, false );
    d->m_label->setText( text );
    d->m_text = text;
    emit textChanged( text );
}


void LabelEditWidget::toggleEditor( bool enabled )
{
    d->toggleEditor( enabled, false );
}


bool LabelEditWidget::eventFilter( QObject* watched, QEvent* event )
{
    if ( watched == d->m_lineEdit &&
         event->type() == QEvent::KeyPress ) {
        QKeyEvent* ke = static_cast<QKeyEvent*>( event );
        if ( ke->key() == Qt::Key_Escape ) {
            d->toggleEditor( false, false );
            emit editingFinished( false );
            return true;
        }
        else if ( ke->key() == Qt::Key_Return ) {
            d->toggleEditor( false, true );
            emit editingFinished( true );
            emit returnPressed();
            return true;
        }
    }
    else if ( watched == d->m_label &&
              event->type() == QEvent::MouseButtonDblClick ) {
        d->toggleEditor( true, true );
        return true;
    }

    return QWidget::eventFilter( watched, event );
}

#include "labeleditwidget.moc"
