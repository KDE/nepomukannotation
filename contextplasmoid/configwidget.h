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

#ifndef _CONFIG_WIDGET_H_
#define _CONFIG_WIDGET_H_

#include <QtGui/QWidget>
#include <QtCore/QList>

#include <Nepomuk/Types/Class>

#include "ui_configwidget.h"

namespace Nepomuk {
    namespace Utils {
        class ClassModel;
    }
}
class QSortFilterProxyModel;
class NepomukContextChooser;

class ConfigWidget : public QWidget,  public Ui::ConfigWidget
{
    Q_OBJECT

public:
    ConfigWidget( NepomukContextChooser*, QWidget* parent  = 0 );
    ~ConfigWidget();

    QList<Nepomuk::Types::Class> selectedTypes() const;

    static QList<Nepomuk::Types::Class> readTypesFromConfig( const KConfigGroup& config );
    static void writeTypesToConfig( KConfigGroup config, const QList<Nepomuk::Types::Class>& );

public Q_SLOTS:
    /// writes back the current config to the annotation menu
    void updateAnnotationMenuConfig();

private Q_SLOTS:
    void slotAvailableTypeSelectionChanged( const QItemSelection& selected, const QItemSelection& );
    void slotSelectedTypeSelectionChanged( const QItemSelection& selected, const QItemSelection& );
    void slotTypeGroupChanged( int pos );
    void slotAddButtonClicked();
    void slotRemoveButtonClicked();
    void slotSortUpButtonClicked();
    void slotSortDownButtonClicked();

private:
    void addSupportedType( const Nepomuk::Types::Class& );

    NepomukContextChooser* m_applet;
    Nepomuk::Utils::ClassModel* m_pimoModel;
    QSortFilterProxyModel* m_pimoSortModel;
};


#endif
