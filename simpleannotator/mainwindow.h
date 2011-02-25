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

#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <QtGui/QMainWindow>
#include <KUrl>

namespace Nepomuk {
    class AnnotationWidget;
    class Resource;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( const KUrl& res );
    ~MainWindow();

private Q_SLOTS:
    void setResource( const Nepomuk::Resource& );

private:
    Nepomuk::AnnotationWidget* m_annotationWidget;
};

#endif
