/*
   Copyright (c) 2008-2009 Sebastian Trueg <trueg@kde.org>

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

#include "annotationdisplaybrowser.h"
#include "nie.h"
#include "pimo.h"

#include <nepomuk/resourcemanager.h>
#include <nepomuk/thing.h>
#include <nepomuk/variant.h>
#include <nepomuk/property.h>
#include <nepomuk/class.h>

#include <Soprano/Model>
#include <Soprano/Util/SignalCacheModel>

#include <QtCore/QTextStream>
#include <QtCore/QList>
#include <QtCore/QStack>

#include <kdebug.h>
#include <kiconloader.h>


namespace {
    QString typesToHtml( const QList<QUrl>& types )
    {
        QList<Nepomuk::Types::Class> typeClasses;
        foreach( const QUrl& type, types ) {
            typeClasses << Nepomuk::Types::Class( type );
        }

        // remove all types that are supertypes of others in the list
        QList<Nepomuk::Types::Class> normalizedTypes;
        for ( int i = 0; i < typeClasses.count(); ++i ) {
            Nepomuk::Types::Class& type = typeClasses[i];
            bool use = true;
            for ( int j = 0; j < typeClasses.count(); ++j ) {
                if ( type != typeClasses[j] &&
                     typeClasses[j].isSubClassOf( type ) ) {
                    use = false;
                    break;
                }
            }
            if ( use ) {
                normalizedTypes << type;
            }
        }

        // extract the labels
        QStringList typeStrings;
        for ( int i = 0; i < normalizedTypes.count(); ++i ) {
            typeStrings << normalizedTypes[i].label();
        }

        return typeStrings.join( ", " );
    }
}


class Nepomuk::AnnotationDisplayBrowser::Private
{
public:
    Resource m_resource;
    bool m_navigationEnabled;
    bool m_propertyRemovalEnabled;

    QStack<QUrl> m_backwardHistory;
    QStack<QUrl> m_forwardHistory;

    QList<Types::Property> m_hiddenProperties;

    QList<Variant> m_valueMap;

    AnnotationDisplayBrowser* q;

    void _k_slotDataChanged();
    void updateDisplay();

    QString createDeleteButton( const Nepomuk::Types::Property& property, const Nepomuk::Variant& value )
    {
        m_valueMap.append( value );
        QUrl deleteQueryUrl( property.uri() );
        deleteQueryUrl.addQueryItem( "delete", QString::number( m_valueMap.count()-1 ) );
        return QString( "<a href=\"%1\"><img src=\"%3\" /></a>" )
            .arg( QString::fromAscii( deleteQueryUrl.toEncoded() ) )
            .arg( KIconLoader::global()->iconPath( "edit-delete", -16 ) );
    }

    QString variantToHtml( const Nepomuk::Types::Property& property, const Nepomuk::Variant& value )
    {
        if ( value.simpleType() == qMetaTypeId<Nepomuk::Resource>() ) {
            QStringList s;
            QList<Nepomuk::Resource> resources = value.toResourceList();
            for ( int i = 0; i < resources.count(); ++i ) {
                QString r = QString( "<a href=\"%1\">%2</a> (%3)" )
                            .arg( QString::fromAscii( resources[i].resourceUri().toEncoded() ) )
                            .arg( resources[i].genericLabel() )
                            .arg( typesToHtml( resources[i].types() ) );
                if ( m_propertyRemovalEnabled )
                    r += " " + createDeleteButton( property, resources[i] );
                s << r;
            }
            return s.join( ", " );
        }
        else if ( value.simpleType() == QVariant::DateTime ) {
            QStringList s;
            QList<QDateTime> dates = value.toDateTimeList();
            for ( int i = 0; i < dates.count(); ++i ) {
                s += KGlobal::locale()->formatDateTime( dates[i], KLocale::FancyShortDate );
            }
            return s.join( ", " );
        }
        else {
            return value.toString();
        }
    }
};


void Nepomuk::AnnotationDisplayBrowser::Private::_k_slotDataChanged()
{
    updateDisplay();
}


void Nepomuk::AnnotationDisplayBrowser::Private::updateDisplay()
{
    m_valueMap.clear();

    QString text;
    QTextStream os( &text );

    os << "<p><b>" << QString( "<a href=\"%1\">%2</a>" )
        .arg( QString::fromAscii( m_resource.resourceUri().toEncoded() ) )
        .arg( m_resource.genericLabel() ) << "</b>";
    os << " (" << typesToHtml( m_resource.types() /*+ m_resource.pimoThing().types()*/ ) << ")</p>";

    os << "<p><table>";

    QHash<QUrl, Nepomuk::Variant> props = m_resource.properties();
//    props.unite( m_resource.pimoThing().properties() );
    for ( QHash<QUrl, Nepomuk::Variant>::const_iterator it = props.constBegin();
          it != props.constEnd(); ++it ) {

        Nepomuk::Types::Property p( it.key() );
//         if ( p == Nepomuk::Vocabulary::NIE::hasLogicalPart() ||
//              p.isSubPropertyOf( Nepomuk::Vocabulary::NIE::hasLogicalPart() ) ) {

//         }
//         else {
           if ( it.key() != Nepomuk::Vocabulary::PIMO::groundingOccurrence() &&
                !m_hiddenProperties.contains( p ) ) {
                kDebug() << p << "not in" << m_hiddenProperties;
                const Nepomuk::Variant& value = it.value();
                os << "<tr><td align=right><i>"
                   << ( p.label().isEmpty() ? p.name() : p.label() )
                   << "</i></td><td width=16px></td><td>" << variantToHtml( p, value ) << "</td></tr>";
            }
//        }
    }
    os << "</table></p>";

    q->setHtml( text );
}


Nepomuk::AnnotationDisplayBrowser::AnnotationDisplayBrowser( QWidget* parent )
    : KTextBrowser( parent ),
      d( new Private() )
{
    d->m_navigationEnabled = false;
    d->m_propertyRemovalEnabled = false;
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
}


Nepomuk::AnnotationDisplayBrowser::~AnnotationDisplayBrowser()
{
    delete d;
}


void Nepomuk::AnnotationDisplayBrowser::setResource( const Resource& resource )
{
    d->m_resource = resource;
    d->updateDisplay();
    emit backwardAvailable( !d->m_backwardHistory.isEmpty() );
    emit forwardAvailable( !d->m_forwardHistory.isEmpty() );
}


Nepomuk::Resource Nepomuk::AnnotationDisplayBrowser::resource() const
{
    return d->m_resource;
}


void Nepomuk::AnnotationDisplayBrowser::setSource( const QUrl& url )
{
    if ( url.hasQueryItem( "delete" ) ) {
        QUrl prop( url );
        prop.removeAllQueryItems( "delete" );
        d->m_resource.removeProperty( prop, d->m_valueMap[url.queryItemValue( "delete" ).toInt()] );
    }
    else if ( d->m_navigationEnabled ) {
        d->m_backwardHistory << d->m_resource.resourceUri();
        d->m_forwardHistory.clear();
        setResource( url );
        emit displayedResourceChanged( d->m_resource );
    }
    else {
        KTextBrowser::setSource( url );
    }
}


void Nepomuk::AnnotationDisplayBrowser::setNavigationEnabled( bool enabled )
{
    d->m_navigationEnabled = enabled;
}


void Nepomuk::AnnotationDisplayBrowser::setPropertyRemovalEnabled( bool enabled )
{
    d->m_propertyRemovalEnabled = enabled;
    d->updateDisplay();
}


bool Nepomuk::AnnotationDisplayBrowser::isNavigationEnabled() const
{
    return d->m_navigationEnabled;
}


bool Nepomuk::AnnotationDisplayBrowser::isPropertyRemovalEnabled() const
{
    return d->m_propertyRemovalEnabled;
}


void Nepomuk::AnnotationDisplayBrowser::backward()
{
    if ( !d->m_backwardHistory.isEmpty() ) {
        QUrl url = d->m_backwardHistory.pop();
        d->m_forwardHistory.push( d->m_resource.resourceUri() );
        setResource( url );
        emit displayedResourceChanged( d->m_resource );
    }
}


void Nepomuk::AnnotationDisplayBrowser::forward()
{
    if ( !d->m_forwardHistory.isEmpty() ) {
        QUrl url = d->m_forwardHistory.pop();
        d->m_backwardHistory.push( d->m_resource.resourceUri() );
        setResource( url );
        emit displayedResourceChanged( d->m_resource );
    }
}


void Nepomuk::AnnotationDisplayBrowser::setHiddenProperties( const QList<Nepomuk::Types::Property>& properties )
{
    d->m_hiddenProperties = properties;
    d->updateDisplay();
}


void Nepomuk::AnnotationDisplayBrowser::addHiddenProperty( const Nepomuk::Types::Property& property )
{
    d->m_hiddenProperties.append( property );
    d->updateDisplay();
}


QList<Nepomuk::Types::Property> Nepomuk::AnnotationDisplayBrowser::hiddenProperties() const
{
    return d->m_hiddenProperties;
}

#include "annotationdisplaybrowser.moc"
