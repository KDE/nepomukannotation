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

#include "files.h"

#include <kiconloader.h>
#include <nepomuk/resource.h>
#include <nepomuk/resourcemanager.h>
#include <nepomuk/tag.h>
#include <soprano/soprano.h>
#include <klocale.h>
#include <KFileItem>

#include "annotationresult.h"

#define ICON_NAME "application-x-zerosize"

using namespace Nepomuk;

FilesAnnotationPlugin::FilesAnnotationPlugin(QObject* parent, const QList<QVariant>&) : AnnotationPlugin(parent)
{
	m_soprano = new Soprano::Util::AsyncModel(Nepomuk::ResourceManager::instance()->mainModel());
}

bool FilesAnnotationPlugin::isReady() const
{
	return true;
}

QString FilesAnnotationPlugin::name() const
{
	return i18n("Files");
}

QString FilesAnnotationPlugin::iconName() const
{
	return QString(ICON_NAME);
}

AnnotationResult* FilesAnnotationPlugin::getResources(const QString& filter)
{
	AnnotationResult* result = AnnotationPlugin::createResult(this);
	result->setReady();
	return result;
}

AnnotationResult* FilesAnnotationPlugin::getResources(const QString& filter, const Types::Property& /*property*/, const Soprano::Node& /*object*/) {
	AnnotationResult* result = AnnotationPlugin::createResult(this);
	result->setReady();
	return result;
}

AnnotationResult* FilesAnnotationPlugin::getResource(const QUrl& resourceUri)
{
	AnnotationResult* result = AnnotationPlugin::createResult(this);

	Nepomuk::Resource res(resourceUri);
	QList<AnnotationResource> resources;
	if(res.exists() && res.resourceType() == Soprano::Vocabulary::Xesam::File()) {
		resources << AnnotationResource(this, res, resourceUri.toString().split("/").last(), QString(), ICON_NAME); // FIXME: suboptimal name
	}

	result->setResources(resources);
	result->setReady();
	return result;
}

AnnotationResult* FilesAnnotationPlugin::getResourcesRelatedTo(const QUrl& resourceUri)
{
	AnnotationResult* result = AnnotationPlugin::createResult(this);

	QString query = QString("select ?uid ?url where {"
							"?uid a <%1> ."
							"?uid <%2> ?url ."
							"?property <%3> <%4> ."
							"{{?uid ?property <%5>} UNION {<%5> ?property ?uid}} }")
							.arg(Soprano::Vocabulary::Xesam::File().toString(),
								Soprano::Vocabulary::Xesam::url().toString(),
								Soprano::Vocabulary::RDFS::subPropertyOf().toString(),
								Soprano::Vocabulary::NAO::annotation().toString(),
								resourceUri.toString());
	qDebug() << query;
	Soprano::Util::AsyncResult* sopranoResult = m_soprano->executeQueryAsync(query, Soprano::Query::QueryLanguageSparql);
	connect(sopranoResult, SIGNAL(resultReady(Soprano::Util::AsyncResult*)), this, SLOT(files_relatedTo_cb(Soprano::Util::AsyncResult*)));

	m_resultsTable[sopranoResult] = result;

	return result;
}

QList<AnnotationProperty> FilesAnnotationPlugin::getProperties(const QString& /*mimetype*/)
{
	QList<AnnotationProperty> properties;
	return properties;
}

QList<AnnotationProperty> FilesAnnotationPlugin::getProperties(const QUrl& /*rdfType*/)
{
	return getProperties(QString());
}

void FilesAnnotationPlugin::files_relatedTo_cb(Soprano::Util::AsyncResult* result)
{
	QList<AnnotationResource> res;
	foreach(const Soprano::BindingSet& set, result->value().value<Soprano::QueryResultIterator>().allBindings()) {
		QString filename = set["url"].literal().toString().split("/").last();
		res.append(AnnotationResource (this, set["uid"].uri(), filename, QString(), QString(ICON_NAME)));
	}

	AnnotationResult* r = m_resultsTable[result];
	r->setResources(res);
	r->setReady();
	m_resultsTable.remove(result);
}

Q_DECLARE_METATYPE(Soprano::QueryResultIterator)

NEPOMUK_EXPORT_ANNOTATION_PLUGIN( Nepomuk::FilesAnnotationPlugin, "nepomuk_filesannotationplugin" )

#include "files.moc"
