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

#include "project.h"

#include <Nepomuk/Resource>
#include <Nepomuk/ResourceManager>
#include <soprano/soprano.h>
#include <soprano/asyncmodel.h>
#include <klocale.h>

#include "annotationresult.h"

#define PIMO "http://www.semanticdesktop.org/ontologies/2007/11/01/pimo"

using namespace Nepomuk;

ProjectAnnotationPlugin::ProjectAnnotationPlugin(QObject* parent, const QVariantList& /*args*/) : AnnotationPlugin(parent), m_ready(false)
{
	m_soprano = new Soprano::Util::AsyncModel(Nepomuk::ResourceManager::instance()->mainModel());
	fetchProjects();
}

ProjectAnnotationPlugin::~ProjectAnnotationPlugin()
{
}

bool ProjectAnnotationPlugin::isReady() const
{
	return m_ready;
}

QString ProjectAnnotationPlugin::name() const
{
	return i18n("Projects");
}

QString ProjectAnnotationPlugin::iconName() const
{
	return "folder-brown";
}

AnnotationResult* ProjectAnnotationPlugin::getResources(const QString& filter)
{
	AnnotationResult* result = AnnotationPlugin::createResult(this);

	// This function needs the cached data, enqueue the command if we're not ready yet
	if(!isReady()) {
		QHash<QString, QVariant> args;
		args["filter"] = filter;
		enqueueCommand(result, qMakePair(AnnotationPlugin::command_getResources, args));
		return result;
	}

	QList<AnnotationResource> res;
	foreach(const AnnotationResource& r, m_resources) {
		if(r.label().contains(filter, Qt::CaseInsensitive) || r.description().contains(filter, Qt::CaseInsensitive))
			res.append(r);
	}

	result->setResources(res);
	result->setReady();
	return result;
}

AnnotationResult* ProjectAnnotationPlugin::getResources(const QString& filter, const Types::Property& property, const Soprano::Node& object)
{
	AnnotationResult* result = AnnotationPlugin::createResult(this);
	result->setReady();
	return result;
}

AnnotationResult* ProjectAnnotationPlugin::getResource(const QUrl& resourceUri)
{
	AnnotationResult* result = AnnotationPlugin::createResult(this);

	// This function needs the cached data, enqueue the command if we're not ready yet
	if(!isReady()) {
		QHash<QString, QVariant> args;
		args["resourceUri"] = resourceUri;
		enqueueCommand(result, qMakePair(AnnotationPlugin::command_getResource, args));
		return result;
	}

	foreach(const AnnotationResource& r, m_resources) {
		if(r.resource().resourceUri() == resourceUri) {
			result->setResources(QList<Nepomuk::AnnotationResource>() << r);
			result->setReady();
			return result;
		}
	}
	result->setReady();
	return result;
}

AnnotationResult* ProjectAnnotationPlugin::getResourcesRelatedTo(const QUrl& resourceUri)
{
	AnnotationResult* result = AnnotationPlugin::createResult(this);

	QString query = QString("select ?id ?title ?desc where"
							"{ ?id a <" PIMO "#Project> ."
							"?id <%1> ?title ."
							"optional { ?id <%2> ?desc } ."
							"?property <%3> <%4> ."
							"{{?id ?property <%5>} UNION {<%5> ?property ?id}} }")
							.arg(Soprano::Vocabulary::NAO::prefLabel().toString(),
								Soprano::Vocabulary::RDFS::comment().toString(),
								Soprano::Vocabulary::RDFS::subPropertyOf().toString(),
								Soprano::Vocabulary::NAO::annotation().toString(),
								resourceUri.toString());

	qDebug() << query;
	Soprano::Util::AsyncResult* sopranoResult = m_soprano->executeQueryAsync(query, Soprano::Query::QueryLanguageSparql);
	connect(sopranoResult, SIGNAL(resultReady(Soprano::Util::AsyncResult*)), this, SLOT(results_relatedTo_cb(Soprano::Util::AsyncResult*)));

	m_resultsTable[sopranoResult] = result;

	return result;
}

QList<AnnotationProperty> ProjectAnnotationPlugin::getProperties(const QString& /*mimetype*/)
{
	return AnnotationPlugin::getProperties(QString());
}

QList<AnnotationProperty> ProjectAnnotationPlugin::getProperties(const QUrl& /*rdfType*/)
{
	return getProperties(QString());
}

void ProjectAnnotationPlugin::fetchProjects()
{
	QString query = QString("select ?id ?title ?desc where"
							"{ ?id a <" PIMO "#Project> ."
							"  ?id <%1> ?title ."
							"optional { ?id <%2> ?desc } }")
							.arg(Soprano::Vocabulary::NAO::prefLabel().toString(), Soprano::Vocabulary::RDFS::comment().toString());
	Soprano::Util::AsyncResult* result = m_soprano->executeQueryAsync(query, Soprano::Query::QueryLanguageSparql);
	connect(result, SIGNAL(resultReady(Soprano::Util::AsyncResult*)), this, SLOT(results_cb(Soprano::Util::AsyncResult*)));
}

void ProjectAnnotationPlugin::results_cb(Soprano::Util::AsyncResult* results)
{
	m_resources.clear();

    Soprano::QueryResultIterator it = results->value().value<Soprano::QueryResultIterator>();
    while(it.next()) {
		m_resources.append(AnnotationResource(this,
                                              Resource(it.binding("id").uri()),
                                              it.binding("title").literal().toString(),
                                              it.binding("desc").isEmpty() ? QString() : it.binding("desc").literal().toString(),
                                              QString("folder-brown"))); // TODO Project icon
	}

	m_ready = true;
	executeQueuedCommands();
	emit ready();
}

void ProjectAnnotationPlugin::results_relatedTo_cb(Soprano::Util::AsyncResult* results)
{
	QList<AnnotationResource> res;

	foreach(const Soprano::BindingSet& set, results->value().value<Soprano::QueryResultIterator>().allBindings()) {
		res.append(AnnotationResource(this, Resource(set["id"].uri()),
										set["title"].literal().toString(),
										set["desc"].isEmpty() ? QString() : set["desc"].literal().toString(),
										QString("folder-brown"))); // TODO Project icon
	}

	AnnotationResult* result = m_resultsTable[results];
	result->setResources(res);
	result->setReady();
	m_resultsTable.remove(results);
}

Q_DECLARE_METATYPE(Soprano::QueryResultIterator)

NEPOMUK_EXPORT_ANNOTATION_PLUGIN( Nepomuk::ProjectAnnotationPlugin, "nepomuk_projectannotationplugin" )

#include "project.moc"
