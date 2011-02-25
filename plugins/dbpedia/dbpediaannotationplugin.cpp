/*
   Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "dbpediaannotationplugin.h"

#include "annotationrequest.h"

#include <nepomuk/resource.h>

#include <Soprano/Node>
#include <Soprano/QueryResultIterator>
#include <Soprano/BindingSet>
#include <Soprano/Client/SparqlModel>
#include <Soprano/Util/AsyncResult>
#include <Soprano/Vocabulary/RDFS>

#include <KDebug>

namespace {
QString normalizedLanguageCode(const QString& language) {
    QString lang, country, modifier, charset;
    KGlobal::locale()->splitLocale(language, lang, country, modifier, charset);
    return lang;
}
}

Nepomuk::DbpediaAnnotationPlugin::DbpediaAnnotationPlugin( QObject* parent, const QVariantList& )
    : AnnotationPlugin( parent ),
      m_currentResult( 0 )
{
    kDebug();
    m_dbpediaModel = new Soprano::Client::SparqlModel( "DBpedia.org" );
}


Nepomuk::DbpediaAnnotationPlugin::~DbpediaAnnotationPlugin()
{
    delete m_dbpediaModel;
}


void Nepomuk::DbpediaAnnotationPlugin::doGetPossibleAnnotations( const AnnotationRequest& request )
{
    if ( !request.filter().isEmpty() ) {
        runQuery( request.filter() );
    }
    else if ( request.resource().isValid() ) {
        runQuery( request.resource().genericLabel() );
    }
    else {
        emitFinished();
    }
}


void Nepomuk::DbpediaAnnotationPlugin::runQuery( const QString& text )
{
    if ( m_currentResult )
        m_currentResult->disconnect( this );

    QString query = QString("prefix rdfs:%1 "
                            "select * where { "
                            "?r a ?type . "
                            "?r rdfs:label ?label . "
                            "?r rdfs:comment ?comment . "
                            "FILTER(lang(?label)='%2') . "
                            "FILTER(lang(?comment)='%2') . "
                            "?label bif:contains \"'%3'\" . "
                            "} LIMIT 10" )
                    .arg(Soprano::Node::resourceToN3( Soprano::Vocabulary::RDFS::rdfsNamespace() ),
                         normalizedLanguageCode(KGlobal::locale()->language()),
                         text );
    kDebug() << query;
    m_currentResult = m_dbpediaModel->executeQueryAsync( query );

    connect( m_currentResult, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
             this, SLOT( slotQueryFinished( Soprano::Util::AsyncResult* ) ) );
}


void Nepomuk::DbpediaAnnotationPlugin::slotQueryFinished( Soprano::Util::AsyncResult* result )
{
    m_currentResult = 0;

    kDebug() << result->lastError();

    Soprano::QueryResultIterator it = result->queryResultIterator();
    while ( it.next() ) {
        kDebug() << it.current();
    }

    // TODO: create annotations either as new pimo things that are related to the resource or as
    //       being the resource (ie. an occurrence of resource().pimoThing())

    emitFinished();
}

NEPOMUK_EXPORT_ANNOTATION_PLUGIN( Nepomuk::DbpediaAnnotationPlugin, "nepomuk_dbpediaannotationplugin" )

#include "dbpediaannotationplugin.moc"
