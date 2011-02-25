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

#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KUrl>

#include <Nepomuk/ResourceManager>

#include "annotationplugin.h"
#include "annotationpluginfactory.h"

#include "mainwindow.h"


int main( int argc, char *argv[] )
{
    KAboutData aboutData( "nepomukannotater", "nepomukannotation",
                          ki18n("Nepomuk Simple PIMO Annotation Shell"),
                          "0.1",
                          ki18n("Nepomuk Simple PIMO Annotation Shell"),
                          KAboutData::License_GPL,
                          ki18n("(c) 2008-2011, Sebastian Trüg"),
                          KLocalizedString(),
                          "http://nepomuk.kde.org" );
    aboutData.addAuthor(ki18n("Sebastian Trüg"),ki18n("Maintainer"), "trueg@kde.org");
    aboutData.setProgramIconName( "nepomuk" );

    KCmdLineArgs::init( argc, argv, &aboutData );

    KCmdLineOptions options;
	options.add("+uri", ki18n("The URI of the resource to annotate"));
    KCmdLineArgs::addCmdLineOptions( options );

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    switch (args->count()) {

        case 0:
            KCmdLineArgs::usageError(i18n("Not enough arguments"));
            return -1;

        case 1:
            {
            KApplication app;
            Nepomuk::ResourceManager::instance()->init();
            MainWindow mainWin( args->url( 0 ) );
            mainWin.show();
            return app.exec();
            }

        default:
            KCmdLineArgs::usageError(i18n("Too many arguments"));
            return -1;
    }

    return -1;
}
