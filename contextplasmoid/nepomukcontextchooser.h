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

#ifndef NEPOMUKCONTEXTCHOOSER_H
#define NEPOMUKCONTEXTCHOOSER_H

#include <Plasma/Applet>

namespace Nepomuk {
    class Resource;
    class AnnotationMenu;
}
class ConfigWidget;
namespace Plasma {
    class PushButton;
}

class NepomukContextChooser : public Plasma::Applet
{
    Q_OBJECT

public:
    NepomukContextChooser( QObject *parent, const QVariantList &args );
    ~NepomukContextChooser();

    QSizeF sizeHint( Qt::SizeHint which, const QSizeF& constraint = QSizeF() ) const;

    Nepomuk::AnnotationMenu* annotationMenu() const { return m_annotationMenu; }

private Q_SLOTS:
    void slotCurrentContextChanged( const Nepomuk::Resource& res );
    void slotClicked();
    void slotAboutToShow( Nepomuk::AnnotationMenu*, bool );
    void slotStartGinkgo();

private:
    void createConfigurationInterface( KConfigDialog* dlg );

    Nepomuk::AnnotationMenu* m_annotationMenu;
    Plasma::PushButton* m_button;

    ConfigWidget* m_configWidget;
};

#endif
