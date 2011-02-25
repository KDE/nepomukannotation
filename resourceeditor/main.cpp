/*
   Copyright (c) 2009-2011 Sebastian Trueg <trueg@kde.org>

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
#include <KLineEdit>
#include <KLocale>
#include <KDialog>

#include <Nepomuk/Thing>
#include <Nepomuk/ResourceManager>

#include "resourceeditor.h"


int main( int argc, char *argv[] )
{
    KAboutData aboutData( "resourceeditor",
                          "resourceeditor",
                          ki18n("ResourceEditor"),
                          "0.2",
                          ki18n("A Nepomuk resource editor"),
                          KAboutData::License_GPL,
                          ki18n("(c) 2009-2011, Sebastian Trüg"),
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

    Nepomuk::ResourceManager::instance()->init();

    Nepomuk::Resource res = Nepomuk::Resource( args->url( 0 ) );

    KDialog dlg;
    Nepomuk::ResourceEditor* re = new Nepomuk::ResourceEditor( &dlg );
    re->setResource( res );
    dlg.setMainWidget( re );
    dlg.setButtons( KDialog::Ok|KDialog::Cancel );
    dlg.setPlainCaption( res.genericLabel() );
    if(dlg.exec() == QDialog::Accepted) {
        re->save();
    }

    return 0;
}
