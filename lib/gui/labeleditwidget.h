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

#ifndef _LABEL_EDIT_WIDGET_H_
#define _LABEL_EDIT_WIDGET_H_

#include <QtGui/QWidget>

class QEvent;

class LabelEditWidget : public QWidget
{
    Q_OBJECT

public:
    LabelEditWidget( QWidget* parent = 0 );
    ~LabelEditWidget();

    QString text() const;

public Q_SLOTS:
    void setText( const QString& text );
    void toggleEditor( bool enabled );

Q_SIGNALS:
    void textChanged( const QString& text );
    void textEdited( const QString& text );
    void returnPressed();
    void editingFinished( bool accepted );

private:
    bool eventFilter( QObject* watched, QEvent* event );

    class Private;
    Private* const d;

    Q_PRIVATE_SLOT( d, void _k_buttonClicked() )
    Q_PRIVATE_SLOT( d, void _k_editingFinished() )
    Q_PRIVATE_SLOT( d, void _k_textChanged( const QString& text ) )
};

#endif
