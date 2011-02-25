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

#include "imageregion.h"

#include <Nepomuk/Resource>
#include <Nepomuk/ResourceManager>
#include <soprano/soprano.h>
#include <soprano/asyncmodel.h>
#include <klocale.h>

#include "annotationresult.h"
#include "annotationpluginfactory.h"

#define NCO "http://www.semanticdesktop.org/ontologies/2007/03/22/nco#"
#define NIR "http://www.semanticdesktop.org/ontologies/2008/07/04/nir#"

using namespace Nepomuk;

ImageRegionAnnotationPlugin::ImageRegionAnnotationPlugin(QObject* parent, const QList<QVariant>&) : AnnotationPlugin(parent), m_ready(false)
{
	// Set up the slave plugins we need
	m_ncoPlugin = NULL;
	m_ncoPlugin = Nepomuk::AnnotationPluginFactory::instance()->getPluginByLibraryName("nepomuk_ncoannotationplugin");
	Q_ASSERT(m_ncoPlugin != NULL);

	m_filesPlugin = NULL;
	m_filesPlugin = Nepomuk::AnnotationPluginFactory::instance()->getPluginByLibraryName("nepomuk_filesannotationplugin");
	Q_ASSERT(m_filesPlugin != NULL);

	m_soprano = new Soprano::Util::AsyncModel(Nepomuk::ResourceManager::instance()->mainModel());
	fetchResources();
}

ImageRegionAnnotationPlugin::~ImageRegionAnnotationPlugin()
{
}


bool ImageRegionAnnotationPlugin::isReady() const
{
	return m_ready;
}

QString ImageRegionAnnotationPlugin::name() const
{
	return i18n("Image regions");
}

QString ImageRegionAnnotationPlugin::iconName() const
{
	return "image-png";
}

AnnotationResult* ImageRegionAnnotationPlugin::getResourcesRelatedTo(const QUrl& resourceUri)
{
	AnnotationResult* result = AnnotationPlugin::createResult(this);

#if 0
	// This function needs the cached data, enqueue the command if we're not ready yet
	if(!isReady()) {
		QHash<QString, QVariant> args;
		args["resourceUri"] = resourceUri;
		enqueueCommand(result, qMakePair(AnnotationPlugin::command_getResourcesRelatedTo, args));
		return result;
	}
#endif

	foreach(const Soprano::BindingSet& set, m_resources) {
		// If there's a file with that uri, send back the associated contact
		if(set["file"].uri() == resourceUri) {
			return m_ncoPlugin->getResource(set["contact"].uri());
		}

		if(set["contact"].uri() == resourceUri) {
			return m_filesPlugin->getResource(set["file"].uri());
		}
	}

	// Will be reached if there are no results
	result->setReady();
	return result;
}

void ImageRegionAnnotationPlugin::results_cb(Soprano::Util::AsyncResult* results)
{
	m_resources.clear();

	m_resources = results->value().value<Soprano::QueryResultIterator>().allBindings();
	m_ready = true;
	executeQueuedCommands();
	emit ready();
}

void ImageRegionAnnotationPlugin::fetchResources()
{
	QString query = QString("select ?file ?contact where"
							"{ ?file a <%1> ."
							"  ?contact a <" NCO "PersonContact> ."
							"  ?file <" NIR "region> ?region ."
							"  ?region <" NIR "regionDepicts> ?contact }")
							.arg(Soprano::Vocabulary::Xesam::File().toString());
	Soprano::Util::AsyncResult* result = m_soprano->executeQueryAsync(query, Soprano::Query::QueryLanguageSparql);
	connect(result, SIGNAL(resultReady(Soprano::Util::AsyncResult*)), this, SLOT(results_cb(Soprano::Util::AsyncResult*)));
}

Q_DECLARE_METATYPE(Soprano::QueryResultIterator)

NEPOMUK_EXPORT_ANNOTATION_PLUGIN( Nepomuk::ImageRegionAnnotationPlugin, "nepomuk_imageregionannotationplugin" )

#include "imageregion.moc"
