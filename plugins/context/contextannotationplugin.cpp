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

#include "contextannotationplugin.h"

#include "annotationrequest.h"
#include "nepomukcontext.h"
#include "simplepimoannotation.h"
#include "pimo.h"

#include <Nepomuk/Resource>
#include <Nepomuk/Variant>
#include <Nepomuk/Types/Property>
#include <Nepomuk/Types/Class>

#include <Soprano/Node>
#include <Soprano/QueryResultIterator>
#include <Soprano/BindingSet>

#include <KLocale>
#include <KDebug>


Nepomuk::ContextAnnotationPlugin::ContextAnnotationPlugin( QObject* parent, const QVariantList& )
    : AnnotationPlugin( parent )
{
    kDebug();
}


Nepomuk::ContextAnnotationPlugin::~ContextAnnotationPlugin()
{
}


void Nepomuk::ContextAnnotationPlugin::doGetPossibleAnnotations( const AnnotationRequest& request )
{
    if ( request.filter().isEmpty() &&
         Nepomuk::ContextServiceInterface::isAvailable() ) {
        Resource contextRes = Nepomuk::ContextServiceInterface::instance()->currentContext();
        if ( contextRes.isValid() ) {
            kDebug() << "Creating annotation suggestion for the current context";
            SimplePimoAnnotation* anno = new SimplePimoAnnotation( Vocabulary::PIMO::isRelated(), contextRes, this );
            anno->setComment( i18n( "Relate to '%1 (%2)'",
                                    contextRes.genericLabel(),
                                    Nepomuk::Types::Class( contextRes.resourceType() ).label() ) );
            addNewAnnotation(anno);
        }
    }

    emitFinished();
}

NEPOMUK_EXPORT_ANNOTATION_PLUGIN( Nepomuk::ContextAnnotationPlugin, "nepomuk_contextannotationplugin" )

#include "contextannotationplugin.moc"
