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

#include "tags.h"
#include "annotation.h"
#include "pimoplugintools.h"
#include "annotationrequest.h"
#include "tagannotation.h"

#include <kiconloader.h>
#include <nepomuk/resource.h>
#include <nepomuk/resourcemanager.h>
#include <nepomuk/tag.h>
#include <soprano/soprano.h>
#include <klocale.h>
#include <KIcon>
#include <KDebug>


Nepomuk::TagsAnnotationPlugin::TagsAnnotationPlugin(QObject* parent, const QList<QVariant>&)
    : AnnotationPlugin(parent)
{
}

void Nepomuk::TagsAnnotationPlugin::doGetPossibleAnnotations( const AnnotationRequest& request )
{
    if ( request.filter().length() >= 2 ) {
        QString filter = request.filter();
        Resource res = request.resource();

        bool havePerfectMatch = false;
        foreach( const Tag& tag, Tag::allTags() ) {
            QString label = tag.genericLabel();
            if( label.contains(filter, Qt::CaseInsensitive) ||
                tag.genericDescription().contains(filter, Qt::CaseInsensitive)) {

                // if the resource is already tagged, we want the perfect match
                // handling, too.
                if ( !res.tags().contains( tag ) ) {
                    TagAnnotation* anno = new TagAnnotation( tag.genericLabel(), this );
                    if ( !filter.isEmpty() ) {
                        anno->setRelevance( 1.0 - calculateDistance( label, filter ) );
                    }
                    else {
                        anno->setRelevance( 0.0001 );
                    }
                    addNewAnnotation( anno );
                }

                if ( !filter.isEmpty() &&
                     !filter.compare( label, Qt::CaseInsensitive ) ) {
                    havePerfectMatch = true;
                }
            }
        }
        if ( !filter.isEmpty() &&
             !havePerfectMatch ) {
            TagAnnotation* anno = new TagAnnotation( filter, this );
            anno->setLabel( filter );
            anno->setIcon( KIcon( "document-new" ) );
            anno->setRelevance( 0.99 ); // while the relevance is very high, it is lower than perfect matches from other plugins
            addNewAnnotation( anno );
        }
    }

    emitFinished();
}

NEPOMUK_EXPORT_ANNOTATION_PLUGIN( Nepomuk::TagsAnnotationPlugin, "nepomuk_tagsannotationplugin" )

#include "tags.moc"
