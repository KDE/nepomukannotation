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

#include "ui_mainwindow.h"

namespace Nepomuk {
    class AnnotationModel;
    class AnnotationPluginWrapper;
    class AnnotationDelegate;
    class AnnotationRelevanceFilterModel;
    class TextExtractionJob;
}
class KJob;
class QModelIndex;
class QEvent;

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow( const KUrl& res );
    ~MainWindow();

private Q_SLOTS:
    void slotTextExtractionResult( KJob* job );
    void slotSelectionChanged( const QModelIndex& current, const QModelIndex& );
    void slotAnotatorFinished();
    void slotCursorPositionChanged();
    void setResource( Nepomuk::Resource );

private:
    bool eventFilter( QObject* watched, QEvent* event );

    Nepomuk::AnnotationModel* m_annotationModel;
    Nepomuk::AnnotationPluginWrapper* m_annotator;
    Nepomuk::AnnotationDelegate* m_delegate;
    Nepomuk::AnnotationRelevanceFilterModel* m_filterModel;
    Nepomuk::TextExtractionJob* m_textExtractionJob;
};

#endif
