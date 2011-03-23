/*
   This file is part of the Nepomuk KDE project.
   Copyright (C) 2011 Sebastian Trueg <trueg@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) version 3, or any
   later version accepted by the membership of KDE e.V. (or its
   successor approved by the membership of KDE e.V.), which shall
   act as a proxy defined in Section 6 of version 3 of the license.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _NEPOMUK_FILE_ITEM_PLUGIN_H_
#define _NEPOMUK_FILE_ITEM_PLUGIN_H_

#include <kabstractfileitemactionplugin.h>

#include <QtCore>
#include <QtGui/QMenu>

#include <KPluginFactory>
#include <KPluginLoader>

#include <Nepomuk/Resource>

class QWidgetAction;
class KRatingWidget;
namespace Nepomuk {
    class AnnotationMenu;
}

class NepomukFileItemPlugin : public KAbstractFileItemActionPlugin
{
    Q_OBJECT

public:
    NepomukFileItemPlugin( QObject* parent, const QVariantList& args );
    ~NepomukFileItemPlugin();

    QList<QAction*> actions(const KFileItemListProperties &fileItemInfos,
                            QWidget *parentWidget);

private Q_SLOTS:
    void ratingChanged(int rating);

private:
    Nepomuk::AnnotationMenu* m_annotationMenu;
    QWidgetAction* m_rateAction;
    KRatingWidget* m_ratingWidget;

    Nepomuk::Resource m_currentResource;
};

K_PLUGIN_FACTORY(NepomukFileItemPluginFactory, registerPlugin<NepomukFileItemPlugin>();)
K_EXPORT_PLUGIN(NepomukFileItemPluginFactory("nepomukfileitemplugin"))

#endif
