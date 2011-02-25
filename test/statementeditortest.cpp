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

#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KUrl>
#include <KLineEdit>

#include <QtGui/QVBoxLayout>

#include "statementeditor.h"


int main( int argc, char *argv[] )
{
    KAboutData aboutData( "statementeditortest", "statementeditortest",
                          ki18n("Nepomuk StatementEditor Test"),
                          "0.1",
                          ki18n("Nepomuk StatementEditor Test"),
                          KAboutData::License_GPL,
                          ki18n("(c) 2009, Sebastian Trüg"),
                          KLocalizedString(),
                          "http://nepomuk.kde.org" );
    aboutData.addAuthor(ki18n("Sebastian Trüg"),ki18n("Maintainer"), "trueg@kde.org");
    aboutData.setProgramIconName( "nepomuk" );

    KCmdLineArgs::init( argc, argv, &aboutData );

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication app;

    Nepomuk::StatementEditor w;
    w.show();

    return app.exec();
}
