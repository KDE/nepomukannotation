/*
   Copyright (c) 2008-2011 Sebastian Trueg <trueg@kde.org>

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

#include "pimoannotationplugin.h"
#include "simplepimoannotation.h"
#include "pimo.h"
#include "pimoplugintools.h"
#include "annotationrequest.h"

#include <Soprano/Node>
#include <Soprano/LiteralValue>
#include <Soprano/QueryResultIterator>
#include <Soprano/Vocabulary/RDFS>
#include <Soprano/Vocabulary/RDF>
#include <Soprano/Vocabulary/XMLSchema>

#include <Nepomuk/ResourceManager>
#include <Nepomuk/Thing>
#include <Nepomuk/Variant>
#include <Nepomuk/Types/Literal>
#include <Nepomuk/Types/Property>
#include <Nepomuk/Types/Class>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/Query>
#include <Nepomuk/Query/QueryParser>

#include <KPluginFactory>
#include <KPluginLoader>
#include <KDebug>
#include <KIcon>


NEPOMUK_EXPORT_ANNOTATION_PLUGIN( Nepomuk::PimoAnnotationPlugin, "nepomuk_pimoannotationplugin" )


Nepomuk::PimoAnnotationPlugin::PimoAnnotationPlugin( QObject* parent, const QVariantList& )
    : AnnotationPlugin( parent )
{
    kDebug();
    m_queryClient = new Nepomuk::Query::QueryServiceClient(this);
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)),
            this, SLOT(slotNewEntries(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(finishedListing()),
            this, SLOT(slotThingSearchFinished()));
}


Nepomuk::PimoAnnotationPlugin::~PimoAnnotationPlugin()
{
}


void Nepomuk::PimoAnnotationPlugin::doGetPossibleAnnotations( const AnnotationRequest& request )
{
    m_queryClient->close();

    if(!request.filter().isEmpty()) {
        m_resource = request.resource();
        m_filter = request.filter();

        Nepomuk::Query::Query query(
                    Nepomuk::Query::ResourceTypeTerm(Vocabulary::PIMO::Thing())
                    && Nepomuk::Query::LiteralTerm(request.filter()));
        query.setLimit(5);
        query.setFullTextScoringEnabled(true);
        kDebug() << query.toSparqlQuery();
        m_queryClient->query(query);
    }
    else {
        emitFinished();
    }
}


void Nepomuk::PimoAnnotationPlugin::slotNewEntries( const QList<Nepomuk::Query::Result>& entries )
{
    Q_FOREACH(const Query::Result& result, entries) {
        Resource res = result.resource();
        SimplePimoAnnotation* anno = new SimplePimoAnnotation( this );
        anno->setIcon(KIcon(res.genericIcon()));
        anno->setLabel( res.genericLabel() );
        anno->setComment( i18nc( "@action A selected resource is set to be related to value %1",
                                "Related to '%1'", res.genericLabel() ) );
        anno->setRelevance( 1.0 - calculateDistance( res.genericLabel(), m_filter) );
        anno->setProperty( Nepomuk::Vocabulary::PIMO::isRelated() );
        anno->setValue( Nepomuk::Variant( res ) );
        addNewAnnotation( anno );
    }
}


void Nepomuk::PimoAnnotationPlugin::slotThingSearchFinished()
{
    m_queryClient->close();
    emitFinished();
}

#include "pimoannotationplugin.moc"
