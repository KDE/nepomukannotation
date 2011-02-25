/*
   Copyright (c) 2008 Sebastian Trueg <trueg@kde.org>

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

#include "pimotypeannotationplugin.h"
#include "pimotypeannotation.h"
#include "pimo.h"
#include "pimoplugintools.h"
#include "annotationrequest.h"

#include <Nepomuk/ResourceManager>
#include <Nepomuk/Thing>
#include <Nepomuk/Types/Literal>
#include <Nepomuk/Types/Property>

#include <KPluginFactory>
#include <KPluginLoader>
#include <KDebug>

#include <math.h>

K_PLUGIN_FACTORY(PimoTypeAnnotationPluginFactory, registerPlugin<Nepomuk::PimoTypeAnnotationPlugin>();)
K_EXPORT_PLUGIN(PimoTypeAnnotationPluginFactory("nepomuk_pimotypeplugin"))


Nepomuk::PimoTypeAnnotationPlugin::PimoTypeAnnotationPlugin( QObject* parent, const QVariantList& )
    : AnnotationPlugin( parent )
{
}


Nepomuk::PimoTypeAnnotationPlugin::~PimoTypeAnnotationPlugin()
{
}


void Nepomuk::PimoTypeAnnotationPlugin::doGetPossibleAnnotations( const AnnotationRequest& request )
{
    Resource resource = request.resource();
    const QString filter = request.filter();

    kDebug() << filter;

    if ( !filter.isEmpty() &&
         !filter.contains( ':' ) ) {
        // this is not really async. Solutions:
        //    1. use a timer to only handle one anno at the time
        //    2. make the Annotation objects very simple to be used in a thread + a convinience class that creates the thread internally

        bool havePerfectMatch = false;
        Nepomuk::Types::Class thing( Vocabulary::PIMO::Thing() );
        foreach( Nepomuk::Types::Class type, thing.allSubClasses() ) {
            // we simply assume that no one will ever want to type Thing explicitely
            if( useType( type, filter ) ) {
                if ( !resource.pimoThing().isValid() || !resource.pimoThing().hasType( type.uri() ) ) {
                    PimoTypeAnnotation* anno = new PimoTypeAnnotation( this );
                    anno->setType( type );
                    anno->setLabel( type.label() );
                    anno->setComment( i18nc("@action A resource get an additional type",
                                            "Add type: %1", type.label() ) );
                    // FIXME: use usage frequence and such for relevance
                    anno->setRelevance( 1.0 - calculateDistance( type.label(), filter ) );
                    addNewAnnotation( anno );
                }

                // even if we do not emit the perfect match, it was there. We do not want to propose the
                // same again
                if ( !filter.isEmpty() &&
                     !filter.compare( type.label(), Qt::CaseInsensitive ) ) {
                    havePerfectMatch = true;
                }
            }
        }

        // create the Annotation that creates a new pimo type
        if ( !havePerfectMatch ) {
            PimoTypeAnnotation* anno = new PimoTypeAnnotation( this );
            anno->setNewTypeLabel( filter );
            anno->setLabel( filter );
            anno->setComment( i18nc("@action A resource gets an additional type",
                                    "Add type: %1", filter ) );
            anno->setRelevance( 0.99 ); // while the relevance is very high, it is lower than perfect matches from other plugins
            addNewAnnotation( anno );
            kDebug() << "Created annotation for" << filter;
        }
    }

    emitFinished();
}


bool Nepomuk::PimoTypeAnnotationPlugin::useType( Types::Class type, const QString& filter )
{
    // 1. only subclasses of pimo:Thing
    // 2. if we have no filter restrict to leaf classes to avoid having all the weird pimo classes such as agent
    if ( !filter.isEmpty() )
        return type.label().contains( filter, Qt::CaseInsensitive );
    else
        return type.subClasses().isEmpty();
}

#include "pimotypeannotationplugin.moc"
