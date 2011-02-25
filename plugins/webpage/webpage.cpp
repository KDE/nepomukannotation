/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
 * Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>
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

#include "webpage.h"
#include "annotationrequest.h"
#include "simplepimoannotation.h"
#include "pimo.h"
#include "nfo.h"

#include <KIconLoader>
#include <KLocale>
#include <KIO/MimetypeJob>
#include <KIO/Job>
#include <KDebug>

#include <Nepomuk/Resource>
#include <Nepomuk/Variant>
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Types/Property>



Nepomuk::WebpageAnnotationPlugin::WebpageAnnotationPlugin(QObject* parent, const QList<QVariant>&)
    : AnnotationPlugin(parent),
      m_lastJob( 0 )
{
}


Nepomuk::WebpageAnnotationPlugin::~WebpageAnnotationPlugin()
{
}


void Nepomuk::WebpageAnnotationPlugin::doGetPossibleAnnotations( const AnnotationRequest& request )
{
    if ( m_lastJob )
        m_lastJob->disconnect( this );
    m_lastJob = 0;

    QUrl url( request.filter() );
    if ( url.isValid() &&
         ( url.scheme() == "http" ||
           url.path().startsWith( "www." ) ) ) {

        // handle typed in urls
        if ( url.scheme().isEmpty() )
            url = "http://" + url.toString();

        // do a quick request to see if it is a valid web page
        m_lastJob = KIO::mimetype( url, KIO::HideProgressInfo );
        connect( m_lastJob, SIGNAL( result( KJob* ) ), this, SLOT( slotMimetypeResult( KJob* ) ) );
    }
    else {
        emitFinished();
    }
}


void Nepomuk::WebpageAnnotationPlugin::slotMimetypeResult( KJob* )
{
    KIO::MimetypeJob* mtj = m_lastJob;
    m_lastJob = 0;
    if ( !mtj->isErrorPage() &&
         !mtj->error() &&
         mtj->mimetype() == "text/html" ) {
        kDebug() << "Creating annotation for url" << mtj->url();
        SimplePimoAnnotation* anno = new SimplePimoAnnotation( this );
        anno->setLabel( mtj->url().url() );
        anno->setComment( i18n( "Related to web page '%1'", mtj->url().url()) );
        anno->setProperty( Vocabulary::PIMO::isRelated() );
        anno->setValue( Variant( Resource( mtj->url(), Vocabulary::NFO::Website() ) ) );
        addNewAnnotation( anno );
    }
    else {
        kDebug() << "Invalid mimetype or error for url" << mtj->url() << mtj->mimetype();
    }

    emitFinished();
}

NEPOMUK_EXPORT_ANNOTATION_PLUGIN( Nepomuk::WebpageAnnotationPlugin, "nepomuk_webpageannotationplugin" )

#include "webpage.moc"
