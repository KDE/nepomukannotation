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

#ifndef ANNOTATIONPLUGIN_FILES_H
#define ANNOTATIONPLUGIN_FILES_H

#include "annotationplugin.h"
#include "annotationproperty.h"
#include "annotationresource.h"

#include <soprano/soprano.h>
#include <soprano/vocabulary.h>
#include <KPluginFactory>
#include <KPluginLoader>

namespace Soprano {
	namespace Util {
		class AsyncResult;
	}
}

namespace Nepomuk {

class AnnotationResource;
class AnnotationResult;

class FilesAnnotationPlugin : public AnnotationPlugin
{
	Q_OBJECT

public:
	FilesAnnotationPlugin(QObject* parent, const QList<QVariant>&);
	bool isReady() const;

	QString name() const;
	QString iconName() const;

public Q_SLOTS:
	AnnotationResult* getResources(const QString& filter);
	AnnotationResult* getResources(const QString& filter, const Types::Property& property, const Soprano::Node& object);
	AnnotationResult* getResource(const QUrl& resourceUri);
	AnnotationResult* getResourcesRelatedTo(const QUrl& resourceUri);
	virtual QList<AnnotationProperty> getProperties(const QString& mimetype);
	virtual QList<AnnotationProperty> getProperties(const QUrl& rdfType);

private Q_SLOTS:
	void files_relatedTo_cb(Soprano::Util::AsyncResult* result);

private:
	Soprano::Util::AsyncModel* m_soprano;

	QMap<Soprano::Util::AsyncResult*, AnnotationResult*> m_resultsTable; 
};

} // End namespace Nepomuk

#endif
