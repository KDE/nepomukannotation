/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
 * Copyright (c) 2008-2009 Sebastian Trueg <trueg@kde.org>
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

#include "nco.h"
#include "nepomukproxy.h"
#include "annotation.h"

#include <kiconloader.h>
#include <KLocalizedString>
#include <KIcon>

#include <Nepomuk/Resource>
#include <Nepomuk/ResourceManager>


namespace {
    class RegionDepictsAnnotation : public Annotation
    {
    public:
        RegionDepictsAnnotation( const QUrl& person, AnnotationPlugin* parent )
            : Annotation( parent ),
              m_personUri( person ) {
        }

        bool equals( Annotation* other ) const {
            if ( RegionDepictsAnnotation* rda = dynamic_cast<RegionDepictsAnnotation*>( other ) ) {
                return rda->m_personUri == m_personUri;
            }
            else
                return Annotation::equals( other );
        }

    protected:
        void doCreate( Resource res ) {
            NepomukProxy::getInstance()->addRegion( res.resourceUri(), QRectF(0, 0, 0, 0), m_personUri );
            connect( NepomukProxy::getInstance(), SIGNAL( regionAdded() ),
                     this, SLOT( emitFinished() ) );
        }

    private:
        QUrl m_personUri;
    };
}


Nepomuk::NcoAnnotationPlugin::NcoAnnotationPlugin(QObject* parent, const QList<QVariant>&)
    : AnnotationPlugin(parent)
{
}


Nepomuk::NcoAnnotationPlugin::~NcoAnnotationPlugin()
{
}


void Nepomuk::NcoAnnotationPlugin::doGetPossibleAnnotations( const AnnotationRequest& request )
{
    m_filter = request.filter();
	connect( NepomukProxy::getInstance(), SIGNAL(contacts(const QList<Soprano::BindingSet>&)),
             this, SLOT(contacts_cb(const QList<Soprano::BindingSet>&)));
	m_nepomuk->getContacts();
}


void NcoAnnotationPlugin::contacts_cb(const QList<Soprano::BindingSet>& results)
{
    QHash<QUrl, QPair<QString, QString> > resources;

	foreach(const Soprano::BindingSet& set, results) {
		// Nepomuk will return several statements for one contact if it is in several groups
		// To prevent having the same contacts several times in the list, we search for an existing occurence before adding it
        QUrl uid = set["uid"].uri();
        if( resources.contains( uid ) ) {
            resources[uid].second = resources[uid].second + QString(", %1").arg(set["gname"].literal().toString());
		} else {
			resources.insert( uid, qMakePair( set["name"].literal().toString(), set["gname"].literal().toString() ) );
		}
	}

    for( QHash<QUrl, QPair<QString, QString> >::const_iterator it = resources.constBegin();
         it != resources.constEnd(); ++it ) {
        QString label = it->first;
        QString desc = it->second;
        QUrl url = it.key();
		if( label.contains(m_filter, Qt::CaseInsensitive) ||
            description.contains(m_filter, Qt::CaseInsensitive)) {
            RegionDepictsAnnotation* anno = new RegionDepictsAnnotation( uri, this );
            anno->setLabel( label );
            anno->setComment( i18nc( "@action Person X is shown in the selected picture", "%1 is in this picture", label ) );
            anno->setIcon( KIcon( QLatin1String("user-identity") ) );
            addNewAnnotation( anno );
        }
    }
}

NEPOMUK_EXPORT_ANNOTATION_PLUGIN( Nepomuk::NcoAnnotationPlugin, "nepomuk_ncoannotationplugin" )

#include "nco.moc"
