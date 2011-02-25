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

#include "propertycreationplugin.h"
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

#include <math.h>

NEPOMUK_EXPORT_ANNOTATION_PLUGIN( Nepomuk::PropertyCreationAnnotationPlugin, "nepomuk_propertycreationannotationplugin" )

namespace {
    /**
     * QVariant::canConvert does not actually try to convert strings to numbers. It for example
     * returns true for strings like "wurst".
     */
    bool canConvert( const QVariant& value, QVariant::Type type )
    {
        if ( value.type() == type ) {
            return true;
        }

        else if ( value.canConvert( type ) ) {
            bool ok = false;
            switch ( type ) {
            case QVariant::Int:
            case QVariant::UInt:
            case QVariant::LongLong:
            case QVariant::ULongLong:
                value.toString().toLongLong( &ok );
                return ok;

            case QVariant::Double:
                value.toString().toDouble( &ok );
                return ok;

            default:
                return true;
            }
        }

        return false;
    }
}



Nepomuk::PropertyCreationAnnotationPlugin::PropertyCreationAnnotationPlugin( QObject* parent, const QVariantList& )
    : AnnotationPlugin( parent )
{
    kDebug();
    m_queryClient = new Nepomuk::Query::QueryServiceClient(this);
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)),
            this, SLOT(slotNewEntries(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(finishedListing()),
            this, SLOT(slotThingSearchFinished()));
}


Nepomuk::PropertyCreationAnnotationPlugin::~PropertyCreationAnnotationPlugin()
{
}


void Nepomuk::PropertyCreationAnnotationPlugin::doGetPossibleAnnotations( const AnnotationRequest& request )
{
    m_resource = request.resource();
    QString filter = request.filter();

    m_queryClient->close();

    if ( filter.count(':') == 1 &&
         m_resource.isValid() ) {
        const QStringList s = filter.split( ':', QString::SkipEmptyParts);
        if ( s.count() == 2 ) {
            m_propertyLabel = s[0];
            m_valueLabel = s[1];
            Query::QueryParser parser;
            m_matchedProperties = parser.matchProperty(m_propertyLabel);
            if(!m_matchedProperties.isEmpty()) {
//                QString::fromLatin1("select distinct ?p ?r where { ?p a rdf:Property . "
//                                    "?p rdfs:range ?range . "
//                                    "?range rdfs:subClassOf pimo:Thing . "
//                                    "?r a ?range . "
//                                    "?r nao:prefLabel ?l . "
//                                    "?l bif:contains \"'%1'\" . "
//                                    "} LIMIT 5")
//                        .arg(m_valueLabel);
                handleNextProperty();
                return;
            }
        }
    }

    // fallback
    emitFinished();
}


void Nepomuk::PropertyCreationAnnotationPlugin::handleNextProperty()
{
    m_currentProperty = m_matchedProperties.takeFirst();
    const Types::Literal literalRange = m_currentProperty.literalRangeType();

    //
    // If it is a literal range create annotation directly
    //
    if ( literalRange.isValid() ) {
        kDebug() << m_currentProperty.uri() << "has literal range of type" << literalRange.dataType();
        if ( canConvert( m_valueLabel, literalRange.dataType() ) ) {
            SimplePimoAnnotation* anno = new SimplePimoAnnotation( this );
            anno->setLabel( "FIXME" );
            anno->setComment( i18nc( "@action A property (%1) of a selected resource is set to value %2",
                                     "Set '%1' to %2", m_currentProperty.label(), QVariant(m_valueLabel).toString() ) );
            anno->setRelevance( 1.0 - calculateDistance( m_currentProperty.label(), m_propertyLabel ) );
            anno->setProperty( m_currentProperty );
            anno->setValue( Nepomuk::Variant( m_valueLabel ) );
            addNewAnnotation( anno );
        }
        // TODO: how about reusing stuff from the parser to handle special dates and so on.
    }


    //
    // Otherwise we do a query for things to relate
    //
    else {
        Nepomuk::Types::Class range = m_currentProperty.range();

        // we only handle properties which can relate to things
        if ( range == Vocabulary::PIMO::Thing() ||
                range.isSubClassOf( Vocabulary::PIMO::Thing() ) ||
                Types::Class( Vocabulary::PIMO::Thing() ).isSubClassOf( range ) ) {
            Nepomuk::Query::Query query(
                        Nepomuk::Query::ResourceTypeTerm(range)
                        && Nepomuk::Query::LiteralTerm(m_valueLabel));
            query.setLimit(5);
            query.setFullTextScoringEnabled(true);
            m_queryClient->query(query);

            // we are waiting for the query result before continuing
            return;
        }
        else {
            kDebug() << "Ignoring property" << m_currentProperty;
        }
    }


    //
    // Asyncroneously handle the next property
    //
    if(!m_matchedProperties.isEmpty()) {
        QMetaObject::invokeMethod( this, "handleNextProperty", Qt::QueuedConnection );
    }
    else {
        emitFinished();
    }
}


void Nepomuk::PropertyCreationAnnotationPlugin::slotNewEntries( const QList<Nepomuk::Query::Result>& entries )
{
    QVariant value( m_valueLabel );

#if 0
    if ( m_matchedProperties.isEmpty() ) {

        kDebug() << "No property to match. Using property label";

        qreal maxResourceAnnotationRelevance = 0.0;

        // 1. Propose to create a new property with a thing that matches m_valueLabel
        //    score higher than the literal ones

        Q_FOREACH(const Query::Result& result, entries) {
            Resource res = result.resource();
            SimplePimoAnnotation* anno = new SimplePimoAnnotation( this );
            anno->setLabel( "FIXME" );
            anno->setComment( i18nc( "@action A property (%1) of a selected resource is set to value %2",
                                     "Create new relation: Set '%1' to %2", m_propertyLabel, res.genericLabel() ) );
            anno->setRelevance( 0.99 * ( 1.0 - calculateDistance( res.genericLabel(), m_valueLabel ) ) );
            anno->setNewPropertyLabel( m_propertyLabel );
            anno->setValue( Nepomuk::Variant( res ) );
            addNewAnnotation( anno );
            maxResourceAnnotationRelevance = qMax( maxResourceAnnotationRelevance, anno->relevance() );
        }

        // 2. propose the creation of a new property with a literal range (handle int, qreal, and string)

        SimplePimoAnnotation* anno = new SimplePimoAnnotation( this );
        anno->setLabel( "FIXME" );
        // relevance of creating new stuff is never as good as something matching perfectly
        // and creating a new property linking to resources is always better than linking to literals
        anno->setRelevance( maxResourceAnnotationRelevance > 0.0 ? 0.99*maxResourceAnnotationRelevance : 0.99 );
        anno->setNewPropertyLabel( m_propertyLabel );

        if ( canConvert( value, QVariant::Int ) ) {
            anno->setComment( i18nc("@action A property (%1) of a selected resource is set to value %2",
                                    "Create new relation: Set '%1' to %2", m_propertyLabel, value.toInt() ) );
            anno->setValue( Nepomuk::Variant( value.toInt() ) );
        }
        else if ( canConvert( value, QVariant::Double ) ) {
            anno->setComment( i18nc("@action A property (%1) of a selected resource is set to value %2",
                                    "Create new relation: Set '%1' to %2", m_propertyLabel, value.toDouble() ) );
            anno->setValue( Nepomuk::Variant( value.toDouble() ) );
        }
        else {
            anno->setComment( i18nc("@action A property (%1) of a selected resource is set to value %2",
                                    "Create new relation: Set '%1' to %2", m_propertyLabel, value.toString() ) );
            anno->setValue( Nepomuk::Variant( value.toString() ) );
        }

        // FIXME: if we have no thing to relate to, propose to create a new thing instead of a string value

        addNewAnnotation( anno );

        emitFinished();
    }
#endif

    Q_FOREACH(const Query::Result& result, entries) {
        Resource res = result.resource();
        SimplePimoAnnotation* anno = new SimplePimoAnnotation( this );
        anno->setLabel( res.genericLabel() );
        anno->setIcon(KIcon(res.genericIcon()));
        anno->setComment( i18nc( "@action A property (%1) of a selected resource is set to value %2",
                                "Set '%1' to %2", m_currentProperty.label(), res.genericLabel() ) );
        anno->setRelevance( ( 1.0 - calculateDistance( m_currentProperty.label(), m_propertyLabel ) )
                           * ( 1.0 - calculateDistance( res.genericLabel(), m_valueLabel ) ) );
        anno->setProperty( m_currentProperty );
        anno->setValue( Nepomuk::Variant( res ) );
        addNewAnnotation( anno );
    }
}


void Nepomuk::PropertyCreationAnnotationPlugin::slotThingSearchFinished()
{
    m_queryClient->close();

    if(!m_matchedProperties.isEmpty()) {
        QMetaObject::invokeMethod( this, "handleNextProperty", Qt::QueuedConnection );
    }
    else {
        emitFinished();
    }
}

#include "propertycreationplugin.moc"
