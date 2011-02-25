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

#include "geonamesannotationplugin.h"
#include "graphretriever.h"
#include "simpleannotation.h"
#include "pimo.h"
#include "pimoplugintools.h"
#include "annotationrequest.h"

#include <Soprano/Model>
#include <Soprano/Node>
#include <Soprano/QueryResultIterator>
#include <Soprano/Statement>
#include <Soprano/StatementIterator>
#include <Soprano/Vocabulary/RDF>
#include <Soprano/Vocabulary/RDFS>
#include <Soprano/Vocabulary/NRL>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/Vocabulary/XMLSchema>

#include <Nepomuk/Thing>
#include <Nepomuk/Variant>
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Types/Property>
#include <Nepomuk/Types/Class>

#include <KGlobal>
#include <KLocale>
#include <KDebug>
#include <KIcon>

#include <QtCore/QTimer>


namespace {
    /**
     * Can create 2 possible annotations:
     *
     * \li link a resource to a geo thing
     * \li make a resource a geo thing
     */
    class GeoNamesAnnotation : public Nepomuk::Annotation
    {
    public:
        GeoNamesAnnotation( QObject* parent )
            : Annotation( parent ),
              m_createNew( true ) {
            setIcon(KIcon("application-x-marble"));
        }

        QString comment() const {
            if( m_createNew )
                return( i18nc( "@item:listbox Statement about a person or something being located in a city or a country",
                               "is located in %1", label() ) );
            else
                return i18nc( "@item:listbox", "is a %1 (%2)", m_geoFeatureThingType.label(), label() );
        }

        void setGeoNamesFeature( const QUrl& resource, const QString& name, const QUrl& featureClass ) {
            m_geoFeature = resource;
            m_geoFeatureName = name;
            if ( featureClass.fragment() == "P" )
                m_geoFeatureThingType = Nepomuk::Vocabulary::PIMO::City();
            else
                m_geoFeatureThingType = Nepomuk::Vocabulary::PIMO::Country();
        }

        /**
         * If set \p true a new geo resource will be created and linked
         * otherwise the resource in question will simply become a geo resource.
         */
        void setCreateNewResource( bool cnr ) {
            m_createNew = cnr;
        }

        bool equals( Annotation* other ) const {
            // probably not that important since only this plugin can create the annotations
            return Annotation::equals( other );
        }

        bool exists( Nepomuk::Resource res ) const {
            if( m_createNew ) {
                return( res.pimoThing().isValid() &&
                        Nepomuk::ResourceManager::instance()->mainModel()->executeQuery(
                            QString( "ask where { "
                                     "%1 %2 ?l . "
                                     "?l a %3 . "
                                     "?l %4 %5 . }")
                            .arg( Soprano::Node::resourceToN3( res.pimoThing().resourceUri() ) )
                            .arg( Soprano::Node::resourceToN3( Nepomuk::Vocabulary::PIMO::hasLocation() ) )
                            .arg( Soprano::Node::resourceToN3( m_geoFeatureThingType.uri() ) )
                            .arg( Soprano::Node::resourceToN3( Nepomuk::Vocabulary::PIMO::hasOtherRepresentation() ) )
                            .arg( Soprano::Node::resourceToN3( m_geoFeature ) ),
                            Soprano::Query::QueryLanguageSparql ).boolValue() );
            }
            else {
                // TODO: this is not 100%, we also should check if the thing is linked to the feature
                return res.pimoThing().hasType( m_geoFeatureThingType.uri() );
            }
        }

    protected:
        void doCreate( Nepomuk::Resource res ) {
            // determine the thing that is a city or country
            Nepomuk::Thing geoThing;
            if( m_createNew ) {
                // create a new geo thing and use it as location of the resource
                geoThing = Nepomuk::Thing( QUrl(),  m_geoFeatureThingType.uri() );
                geoThing.setLabel( m_geoFeatureName );
                res.pimoThing().addProperty( Nepomuk::Vocabulary::PIMO::hasLocation(), geoThing );
            }
            else {
                // the resource itself becomes a geo thing
                geoThing = res.pimoThing();
                geoThing.addType( m_geoFeatureThingType.uri() );
            }

            // save the link to geonames
            Nepomuk::Resource geoFeatureRes( m_geoFeature, QUrl::fromEncoded( "http://www.geonames.org/ontology#Feature", QUrl::StrictMode ) );
            geoThing.addProperty( Nepomuk::Vocabulary::PIMO::hasOtherRepresentation(), geoFeatureRes );

            emitFinished();
        }

    private:
        QUrl m_geoFeature;
        QString m_geoFeatureName;
        mutable Nepomuk::Types::Class m_geoFeatureThingType;
        bool m_createNew;
    };

    /**
     * geonames does not handle all language variants such as 'en_US' or 'en_GB' but
     * only the basic language codes, in this example 'en'.
     */
    QString normalizedLanguageCode() {
        QString language = KGlobal::locale()->language();
        int p = language.indexOf( '_' );
        if ( p > 0 ) {
            return language.left( p );
        }
        else {
            return language;
        }
    }
}


Nepomuk::GeoNamesAnnotationPlugin::GeoNamesAnnotationPlugin( QObject* parent, const QVariantList& )
    : AnnotationPlugin( parent ),
      m_lastRdfJob( 0 )
{
}


Nepomuk::GeoNamesAnnotationPlugin::~GeoNamesAnnotationPlugin()
{
}


void Nepomuk::GeoNamesAnnotationPlugin::doGetPossibleAnnotations( const AnnotationRequest& request )
{
    // ignore any previous jobs
    if ( m_lastRdfJob ) {
        m_lastRdfJob->disconnect( this );
        m_lastRdfJob = 0;
    }

    QString filter = request.filter();

    // if only a resource is set we try to make it a city or country
    if( filter.isEmpty() ) {
        if( request.resource().isValid() )
            filter = request.resource().genericLabel();
    }

    if ( filter.length() > 2 ) {
        kDebug() << "starting geonames query for" << filter;

        // build geonames query
        QUrl url( "http://api.geonames.org/search" );

        // search for names of things that start with filter
        url.addQueryItem( "name", filter );

        // no more than 5 results
        url.addQueryItem( "maxRows", "5" );

        // default to the user's language if possible
        url.addQueryItem( "lang", normalizedLanguageCode() );

        // get the full info which includes the alt names
        url.addQueryItem( "style", "full" );

        // we only want to handle countries and cities for now
        url.addQueryItem( "featureClass", "P" ); // city, village, ...
        url.addQueryItem( "featureClass", "A" ); // country, state, region, ...

        // and please give us some rdf
        url.addQueryItem( "type", "rdf" );

        // "nepomuk" has been registered as username with geonames
        // this allows a max of 30000 queries per day which effectively
        // kills this plugin. :(
        url.addQueryItem( "username", "nepomuk" );

        kDebug() << url;

        m_lastRdfJob = new GraphRetriever( this );
        connect( m_lastRdfJob, SIGNAL( result( KJob* ) ),
                 this, SLOT( slotGeoNamesQueryResult( KJob* ) ) );
        m_lastRdfJob->setUrl( url );
        m_lastRdfJob->start();
    }
    else {
        emitFinished();
    }
}


void Nepomuk::GeoNamesAnnotationPlugin::slotGeoNamesQueryResult( KJob* job )
{
    if ( !job->error() ) {
        if ( Soprano::Model* model = m_lastRdfJob->model() ) {
            Soprano::QueryResultIterator it = model->executeQuery( QString( "select ?r ?name ?altname ?type ?country where { "
                                                                            "?r a <http://www.geonames.org/ontology#Feature> . "
                                                                            "?r <http://www.geonames.org/ontology#name> ?name . "
                                                                            "?r <http://www.geonames.org/ontology#featureClass> ?type . "
                                                                            "?r <http://www.geonames.org/ontology#countryCode> ?country . "
                                                                            "OPTIONAL { ?r <http://www.geonames.org/ontology#alternateName> ?altname . "
                                                                            "FILTER(LANG(?altname) = '%1') . } }" )
                                                                   .arg( normalizedLanguageCode() ),
                                                                   Soprano::Query::QueryLanguageSparql );
            while ( it.next() ) {
                QUrl featureUri = it.binding( "r" ).uri();
                QString name = it.binding( "name" ).toString();
                QString altname = it.binding( "altname" ).toString();
                QString country = it.binding( "country" ).toString();
                QUrl type = it.binding( "type" ).uri();

                QString displayName = altname.isEmpty() ? name : altname;

                GeoNamesAnnotation* anno = new GeoNamesAnnotation( this );
                anno->setGeoNamesFeature( featureUri, displayName, type );

                // disembiguate city names
                if ( type.fragment() != "A" ) {   // makes no sense to have labels like "country (country)"
                    const QString countryName = KGlobal::locale()->countryCodeToName(country);
                    if ( !countryName.isEmpty() ) {
                        displayName = QString( "%1 (%2)" ).arg( displayName, countryName );
                    }
                }
                anno->setLabel( displayName );

                if( currentRequest().filter().isEmpty() ) {
                    anno->setRelevance( 1.0 - calculateDistance( name, currentRequest().resource().genericLabel() ) );
                    anno->setCreateNewResource( false );
                }
                else {
                    anno->setRelevance( 1.0 - calculateDistance( name, currentRequest().filter() ) );
                }
                addNewAnnotation( anno );
            }

            delete model;
        }
    }

    m_lastRdfJob = 0;

    emitFinished();
}

NEPOMUK_EXPORT_ANNOTATION_PLUGIN( Nepomuk::GeoNamesAnnotationPlugin, "nepomuk_geonamesannotationplugin" )

#include "geonamesannotationplugin.moc"
