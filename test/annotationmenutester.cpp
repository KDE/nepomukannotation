/*
   This file is part of the Nepomuk KDE project.
   Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) version 3, or any
   later version accepted by the membership of KDE e.V. (or its
   successor approved by the membership of KDE e.V.), which shall
   act as a proxy defined in Section 6 of version 3 of the license.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KUrl>
#include <KLineEdit>
#include <Nepomuk/Resource>

#include <QtGui/QCursor>

#include "annotationmenu.h"


int main( int argc, char *argv[] )
{
    KAboutData aboutData( "annotationmenutester", "annotationmenutester",
                          ki18n("Nepomuk AnnotationMenu Test"),
                          "0.1",
                          ki18n("Nepomuk AnnotationMenu Test"),
                          KAboutData::License_GPL,
                          ki18n("(c) 2010, Sebastian Trüg"),
                          KLocalizedString(),
                          "http://nepomuk.kde.org" );
    aboutData.addAuthor(ki18n("Sebastian Trüg"),ki18n("Maintainer"), "trueg@kde.org");
    aboutData.setProgramIconName( "nepomuk" );

    KCmdLineArgs::init( argc, argv, &aboutData );

    KCmdLineOptions options;
	options.add("+uri", ki18n("The URI of the resource to annotate"));
    KCmdLineArgs::addCmdLineOptions( options );
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    KApplication app;

    Nepomuk::AnnotationMenu m;
    m.setConfigurationFlags( Nepomuk::AnnotationMenu::ShowAll|Nepomuk::AnnotationMenu::UseSubMenus );
    if ( args->count() > 0 )
        m.setResource( Nepomuk::Resource( args->url( 0 ) ) );
    m.exec( QCursor::pos() );

    return 0;
}
