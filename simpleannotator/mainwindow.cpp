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

#include "mainwindow.h"
#include "annotationwidget.h"

#include <KLocale>
#include <KDebug>
#include <KMenu>

Q_DECLARE_METATYPE(Nepomuk::Annotation*)


MainWindow::MainWindow( const KUrl& res )
    : QMainWindow()
{
    // setup UI
    setWindowTitle( i18n( "Nepomuk - Annotate Resource" ) );
    m_annotationWidget = new Nepomuk::AnnotationWidget( this );
    setCentralWidget( m_annotationWidget );

    // setup resource
    setResource( Nepomuk::Resource( res ) );
}


MainWindow::~MainWindow()
{
}

void MainWindow::setResource( const Nepomuk::Resource& resource )
{
    m_annotationWidget->setResource(resource);
}

#include "mainwindow.moc"
