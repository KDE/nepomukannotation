/*
   Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>

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

#include "resourcerelationview.h"
#include "pimo.h"
#include "nie.h"
#include "nfo.h"

#include <nepomuk/resourcemanager.h>
#include <nepomuk/thing.h>
#include <nepomuk/variant.h>
#include <nepomuk/property.h>
#include <nepomuk/class.h>

#include <Soprano/Model>
#include <Soprano/Util/SignalCacheModel>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/Vocabulary/Xesam>

#include <kdebug.h>


class Nepomuk::ResourceRelationView::Private
{
public:
    ResourceRelationView* q;

    Resource m_resource;

    QSet<QUrl> m_hiddenProperties;

    void _k_slotDataChanged();
    void updateDisplay();
    void addRelationsFromResource( Resource res );
};


void Nepomuk::ResourceRelationView::Private::_k_slotDataChanged()
{
    updateDisplay();
}


void Nepomuk::ResourceRelationView::Private::updateDisplay()
{
    q->clear();

    addRelationsFromResource( m_resource );

    if ( m_resource.hasType( Nepomuk::Vocabulary::PIMO::Thing() ) ) {
        // HACK: until kdelibs are fixed we need to store the thing to make the grounding occurrence available
        m_resource.setLabel( m_resource.label() );

        Thing thing = m_resource;
        QList<Resource> occurrences = thing.groundingOccurrences() + thing.occurrences();
        foreach( Resource res, occurrences ) {
            addRelationsFromResource( res );
        }
    }
}


void Nepomuk::ResourceRelationView::Private::addRelationsFromResource( Resource resource )
{
    QHash<QUrl, Nepomuk::Variant> properties = resource.properties();
    for ( QHash<QUrl, Nepomuk::Variant>::const_iterator it = properties.constBegin();
          it != properties.constEnd(); ++it ) {
        if ( !m_hiddenProperties.contains( it.key() ) ) {
            if ( it.value().isResource() || it.value().isResourceList() ) {
                foreach( const Nepomuk::Resource& res, it.value().toResourceList() ) {
                    q->addRelation( RelationModel::Relation( resource, it.key(), res ) );
                }
            }
            else {
                q->addRelation( RelationModel::Relation( resource, it.key(), it.value() ) );
            }
        }
    }
}


Nepomuk::ResourceRelationView::ResourceRelationView( QWidget* parent )
    : RelationView( parent ),
      d( new Private() )
{
    d->q = this;

    Soprano::Util::SignalCacheModel* scm = new Soprano::Util::SignalCacheModel( ResourceManager::instance()->mainModel() );
    scm->setParent( this );
    scm->setCacheTime( 500 ); // updating twice a second is enough

    connect( scm,
             SIGNAL( statementsAdded() ),
             this, SLOT( _k_slotDataChanged() ) );
    connect( scm,
             SIGNAL( statementsRemoved() ),
             this, SLOT( _k_slotDataChanged() ) );

    // default hidden properties
    d->m_hiddenProperties << Nepomuk::Vocabulary::PIMO::groundingOccurrence()
                          << Nepomuk::Vocabulary::PIMO::occurrence()
                          << Nepomuk::Vocabulary::NIE::plainTextContent()
                          << Soprano::Vocabulary::NAO::prefLabel()
                          << Soprano::Vocabulary::NAO::description()
                          << Soprano::Vocabulary::Xesam::sourceModified()
                          << Soprano::Vocabulary::NAO::created()
                          << Soprano::Vocabulary::NAO::lastModified()
                          << QUrl( "http://strigi.sf.net/ontologies/0.9#depth" );
}


Nepomuk::ResourceRelationView::~ResourceRelationView()
{
    delete d;
}


void Nepomuk::ResourceRelationView::setResource( const Nepomuk::Resource& resource )
{
    d->m_resource = resource;
    d->updateDisplay();
}

#include "resourcerelationview.moc"
