/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
 *               2008-2010 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef NEPOMUKMENUPLUGIN_H
#define NEPOMUKMENUPLUGIN_H

#include <konq_popupmenu.h>
#include <konq_popupmenuplugin.h>
#include <QtCore>
#include <QtGui/QMenu>
#include <KPluginFactory>
#include <KPluginLoader>
#include <nepomuk/resource.h>

class KHTMLView;
class KRatingWidget;
namespace Nepomuk {
    class AnnotationMenu;
}

class NepomukMenuPlugin : public KonqPopupMenuPlugin
{
	Q_OBJECT

public:
	NepomukMenuPlugin( QObject* parent, const QVariantList& args );
	~NepomukMenuPlugin();

	// Called on every right click
	virtual void setup( KActionCollection* collection,
                        const KonqPopupMenuInformation& info,
                        QMenu* menu );

private Q_SLOTS:
	void ratingChanged(unsigned int rating);
    void slotResourceAnnotated();

private:
	/**
     * Saves the current page into Nepomuk
     * (pimo:Document and nfo:Website).
     * \return the pimo:Thing resource.
     */
	Nepomuk::Resource saveCurrentPage();

    Nepomuk::AnnotationMenu* m_annotationMenu;
    QWidgetAction* m_rateAction;
	KRatingWidget* m_ratingWidget;

    // pointer to the html view (can be 0)
	KHTMLView*     m_view;

    // The URI of the webpage currenly annotated
    QUrl           m_currentUri;
};

K_PLUGIN_FACTORY(NepomukMenuPluginFactory, registerPlugin<NepomukMenuPlugin>();)
K_EXPORT_PLUGIN(NepomukMenuPluginFactory("nepomukmenuplugin"))

#endif
