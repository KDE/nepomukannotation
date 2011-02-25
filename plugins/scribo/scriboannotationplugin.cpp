/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (c) 2009-2011 Sebastian Trueg <trueg@kde.org>
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

#include "scriboannotationplugin.h"
#include "occurrenceannotation.h"
#include "eventannotation.h"
#include "textextractionjob.h"

#include "nie.h"

#include <scribo/textmatcher.h>
#include <scribo/textmatch.h>
#include <scribo/textoccurrence.h>
#include <scribo/entity.h>

#include "tagannotation.h"
#include "simpleannotation.h"
#include "simplepimoannotation.h"
#include "stackedpimotextannotation.h"
#include "annotationrequest.h"
#include "pimo.h"

#include <Nepomuk/ResourceManager>
#include <Nepomuk/Tag>
#include <Nepomuk/Variant>
#include <Nepomuk/Types/Property>
#include <Nepomuk/Types/Class>
#include <Nepomuk/Query/Query>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/LiteralTerm>

#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Soprano/Node>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/Vocabulary/RDF>
#include <Soprano/Index/IndexFilterModel>

#include <KDebug>
#include <KLocale>
#include <KIcon>


Nepomuk::ScriboAnnotationPlugin::ScriboAnnotationPlugin( QObject* parent, const QList<QVariant>& )
    : AnnotationPlugin( parent )
{
    m_matcher = new Scribo::TextMatcher( this );
    connect( m_matcher, SIGNAL( newMatch( Scribo::TextMatch ) ),
             this, SLOT( slotNewMatch( Scribo::TextMatch ) ) );
    connect( m_matcher, SIGNAL( finished() ),
             this, SLOT( slotMatchingFinished() ) );
}


void Nepomuk::ScriboAnnotationPlugin::doGetPossibleAnnotations( const AnnotationRequest& request )
{
    kDebug();
    // get the text from the resource
    // TODO: cancel the previous job
    if(request.resource().isValid()) {
        TextExtractionJob* job = new TextExtractionJob(this);
        connect(job, SIGNAL(result(KJob*)),
                this, SLOT(slotTextExtractionResult(KJob*)));
        job->setResource(request.resource());
        job->start();
    }
    else {
        emitFinished();
    }
}


void Nepomuk::ScriboAnnotationPlugin::slotTextExtractionResult(KJob* job)
{
    const QString text = static_cast<TextExtractionJob*>(job)->text();
    if(text.isEmpty()) {
        emitFinished();
    }
    else {
        m_matcher->getPossibleMatches( text );
    }
}

void Nepomuk::ScriboAnnotationPlugin::slotMatchingFinished()
{
    kDebug();

    if ( currentRequest().filter().isEmpty() ) {
        QList<Scribo::TextMatch> matches = m_matcher->matches();

        // let's see if we can find some nice matches that we can combine
        // HACK: this is a bit of a hack to show what is possible. It needs to become more generic

        // first let's see if we have a date or a location in there which we could use as a basis for an event
        QList<Scribo::Statement> dates;
        QList<Scribo::Entity> locations;
        QList<Scribo::Entity> projects;
        foreach( const Scribo::TextMatch& match, matches ) {
            if ( match.isStatement() ) {
                Scribo::Statement statement = match.toStatement();
                if ( statement.value().isDate() )
                    dates << statement;
            }
            else if ( match.isEntity() ) {
                Scribo::Entity entity = match.toEntity();
                if ( entity.type().uri() == Nepomuk::Vocabulary::PIMO::Location() ||
                     entity.type().isSubClassOf( Nepomuk::Vocabulary::PIMO::Location() ) ) {
                    locations << entity;
                }
                else if ( entity.type().uri() == Nepomuk::Vocabulary::PIMO::Project() ) {
                    projects << entity;
                }
            }
        }

        if ( !locations.isEmpty() || !dates.isEmpty() ) {
            kDebug() << "found locations or dates. Creating event annotation";
            // FIXME: create an annotation for each text position (although that would look weird in the annoation list)
            EventAnnotation* anno = new EventAnnotation();
            anno->setDates( dates );
            anno->setLocations( locations );
            anno->setProjects( projects );
            addNewAnnotation( anno );
        }
        else {
            kDebug() << "no locations or dates found:" << dates.count() << locations.count() << projects.count();
        }
    }

    emitFinished();
}


void Nepomuk::ScriboAnnotationPlugin::slotNewMatch( const Scribo::TextMatch& match )
{
    // FIXME: handle statements

    // we apply the filter in a very stupid manner just to make sure we do not provide
    // the AnnotationWidget with stupid results
    if ( match.isEntity() &&
        ( currentRequest().filter().isEmpty() ||
          match.label().toLower().contains( currentRequest().filter().toLower() ) ) ) {
        Scribo::Entity entity = match.toEntity();

        if ( entity.localResource().isValid() ) {
            createAnnotationForResource( entity, entity.localResource(), 1.0 );
        }
        else if( !entity.label().isEmpty() ) {
            // FIXME: use the entity type

            Query::Query query(
                Query::AndTerm(
                    Query::LiteralTerm( entity.label() ),
                    Query::OrTerm(
                        Query::ResourceTypeTerm( Soprano::Vocabulary::NAO::Tag() ),
                        Query::ResourceTypeTerm( Vocabulary::PIMO::Project() ),
                        Query::ResourceTypeTerm( Vocabulary::PIMO::Task() ),
                        Query::ResourceTypeTerm( Vocabulary::PIMO::Person() )
                        )
                    )
                );

            bool haveTag = false;
            Soprano::QueryResultIterator it = ResourceManager::instance()->mainModel()->executeQuery( query.toSparqlQuery(), Soprano::Query::QueryLanguageSparql );
            while ( it.next() ) {
                Nepomuk::Resource res( it[0].uri() );
                double score = 0.0; //it[1].literal().toDouble();

                kDebug() << res.resourceUri() << score;
                if ( res.resourceType() == Soprano::Vocabulary::NAO::Tag() )
                    haveTag = true;
                createAnnotationForResource( entity, res, score );
            }

            if ( !haveTag ) {
#ifdef WE_DISABLE_THE_LINKING_TO_TEXT_PARTS_UNTIL_NIE_HASPART_IS_SUPPORTED_EVERYWHERE
                // FIXME: support all occurrences
                Nepomuk::StackedPimoTextAnnotation* anno = new Nepomuk::StackedPimoTextAnnotation();
                anno->setChildAnnotation( new TagAnnotation( entity.label() ) );
                anno->setTextOffset( match.occurrences().first().startPosition() );
                anno->setTextLength( match.occurrences().first().length() );
                anno->setTextExcerpt( match.label() );
                anno->setRelevance( 0.5*match.occurrences().first().relevance() ); // FIXME: this is awful
                addNewAnnotation( anno );
#else
                Nepomuk::TagAnnotation* anno = new Nepomuk::TagAnnotation( entity.label() );
                anno->setRelevance( 0.5*match.occurrences().first().relevance() ); // FIXME: this is awful
                addNewAnnotation(anno);
#endif
            }
        }
        else {
            kDebug() << "Ignoring entity without label!";
        }
    }
}


void Nepomuk::ScriboAnnotationPlugin::createAnnotationForResource( const Scribo::Entity& entity, const Nepomuk::Resource& res, double score )
{
    QUrl type = res.resourceType();
    QString label = res.genericLabel();
    double relevance = entity.occurrences().first().relevance();

#ifdef WE_DISABLE_THE_LINKING_TO_TEXT_PARTS_UNTIL_NIE_HASPART_IS_SUPPORTED_EVERYWHERE
    Nepomuk::StackedTextAnnotation* anno = 0;
    if ( type == Soprano::Vocabulary::NAO::Tag() ) {
        anno = new Nepomuk::StackedPimoTextAnnotation();
        anno->setChildAnnotation( new TagAnnotation( res.genericLabel() ) );
    }
    else {
        anno = new Nepomuk::StackedTextAnnotation();
        Nepomuk::Types::Class typeClass( type );
        anno->setLabel( label );
        anno->setComment( i18n( "Mentions '%1' (%2)", label, typeClass.label() ) );
        anno->setIcon( KIcon( "nepomuk" ) );

        OccurrenceAnnotation* sAnno = new OccurrenceAnnotation( res );
        anno->setChildAnnotation( sAnno );
    }

    if ( anno ) {
        // FIXME: support all occurrences
        anno->setRelevance( score * relevance /*1.0/( double )pos*/ ); // FIXME: this is awful
        anno->setTextOffset( entity.occurrences().first().startPosition() );
        anno->setTextLength( entity.occurrences().first().length() );
        anno->setTextExcerpt( entity.label() );
        addNewAnnotation( anno );
    }
#else
    if ( type == Soprano::Vocabulary::NAO::Tag() ) {
        Nepomuk::TagAnnotation* anno = new Nepomuk::TagAnnotation( res.genericLabel() );
        anno->setRelevance( score * relevance /*1.0/( double )pos*/ ); // FIXME: this is awful
        addNewAnnotation( anno );
    }
    else {
        Nepomuk::SimplePimoAnnotation* anno = new Nepomuk::SimplePimoAnnotation( Nepomuk::Vocabulary::PIMO::isRelated(), res );
        Nepomuk::Types::Class typeClass( type );
        anno->setLabel( label );
        anno->setComment( i18n( "Mentions '%1' (%2)", label, typeClass.label() ) );
        anno->setIcon( KIcon( "nepomuk" ) );
        anno->setRelevance( score * relevance /*1.0/( double )pos*/ ); // FIXME: this is awful
        addNewAnnotation(anno);
    }
#endif
}

NEPOMUK_EXPORT_ANNOTATION_PLUGIN( Nepomuk::ScriboAnnotationPlugin, "nepomuk_scriboannotationplugin" )

#include "scriboannotationplugin.moc"
