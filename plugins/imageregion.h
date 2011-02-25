/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
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

#ifndef ANNOTATIONPLUGIN_WEBPAGE_H
#define ANNOTATIONPLUGIN_WEBPAGE_H

#include "annotationplugin.h"
#include "annotationproperty.h"
#include "annotationresource.h"

#include <soprano/soprano.h>

namespace Nepomuk {

/**
 * \class ImageRegionAnnotationPlugin webpage.cpp ImageRegionAnnotationPlugin
 * 
 * \brief Annnotation plugins which handles the pimo:Docuemnt type when
 * it is used to describe webpages. All those pimo:Document instances
 * must have a nfo:Website as a grounding occurrence.
 *
 * \author Adrien Bustany <madcat@mymadcat.com>
 */
class ImageRegionAnnotationPlugin : public AnnotationPlugin
{
	Q_OBJECT

public:
	ImageRegionAnnotationPlugin(QObject* parent, const QList<QVariant>&);
	~ImageRegionAnnotationPlugin();

	bool isReady() const;

	QString name() const;
	QString iconName() const;

public Q_SLOTS:
	AnnotationResult* getResourcesRelatedTo(const QUrl& resourceUri);
	
private Q_SLOTS:
	void results_cb(Soprano::Util::AsyncResult* results);

private:
	void fetchResources();
	Soprano::Util::AsyncModel* m_soprano;
	QList<Soprano::BindingSet> m_resources;
	QMap<Soprano::Util::AsyncResult*, QString> m_queryFilters;
	QMap<Soprano::Util::AsyncResult*, AnnotationResult*> m_resultsTable;
	bool m_ready;

	AnnotationPlugin* m_ncoPlugin;
	AnnotationPlugin* m_filesPlugin;
};

} // End namespace Nepomuk

#endif
