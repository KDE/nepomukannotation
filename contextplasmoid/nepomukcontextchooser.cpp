/* This file is part of the Nepomuk Project
   Copyright (c) 2010 Sebastian Trueg <trueg@kde.org>

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

#include "nepomukcontextchooser.h"
#include "configwidget.h"
#include "nepomukcontext.h"
#include "annotationmenu.h"

#include <Plasma/PushButton>
#include <Plasma/ToolTipManager>
#include <Plasma/ToolTipContent>

#include <KIcon>
#include <KLocale>
#include <KDebug>
#include <KConfigDialog>
#include <KPushButton>
#include <KStandardDirs>

#include <QtGui/QToolButton>
#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QCursor>
#include <QtCore/QProcess>


NepomukContextChooser::NepomukContextChooser(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
      m_configWidget( 0 )
{
    setHasConfigurationInterface( true );

    m_button = new Plasma::PushButton( this );
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout( this );
    layout->addItem( m_button );
    layout->setContentsMargins( 0.0, 0.0, 0.0, 0.0 );

    m_annotationMenu = new Nepomuk::AnnotationMenu();
    connect( m_annotationMenu, SIGNAL( aboutToShow( Nepomuk::AnnotationMenu*, bool ) ),
             this, SLOT( slotAboutToShow( Nepomuk::AnnotationMenu*, bool ) ) );

    // trueg: setting the menu brings it up in the wrong place
    // using QCursor::pos() is not perfect either but much better already.
//    m_button->nativeWidget()->setMenu( m_annotationMenu );
    connect( m_button->nativeWidget(), SIGNAL( clicked() ),
             this, SLOT( slotClicked() ) );

    slotCurrentContextChanged( Nepomuk::ContextServiceInterface::instance()->currentContext() );
    connect( Nepomuk::ContextServiceInterface::instance(), SIGNAL( currentContextChanged( Nepomuk::Resource ) ),
             this, SLOT( slotCurrentContextChanged( Nepomuk::Resource ) ) );

    // read config
    QList<Nepomuk::Types::Class> types = ConfigWidget::readTypesFromConfig( config() );
    if ( !types.isEmpty() )
        m_annotationMenu->setSupportedTypes( types );
}


NepomukContextChooser::~NepomukContextChooser()
{
}


QSizeF NepomukContextChooser::sizeHint( Qt::SizeHint which, const QSizeF& constraint ) const
{
    QSizeF hint = Applet::sizeHint( which, constraint );
    if ( which == Qt::MinimumSize ||
         which == Qt::PreferredSize ) {
        hint.setWidth( qMax( hint.width(), qreal( QFontMetrics(font()).width( "XXXXXXXXXXXXX" ) ) ) );
    }
    return hint;
}


void NepomukContextChooser::slotCurrentContextChanged( const Nepomuk::Resource& res )
{
    QString icon = res.genericIcon();
    if ( icon.isEmpty() )
        icon = QLatin1String( "nepomuk" );

    m_button->nativeWidget()->setIcon( KIcon( icon ) );

    if ( res.isValid() ) {
        m_button->nativeWidget()->setText( res.genericLabel() );
        Plasma::ToolTipManager::self()->setContent(
            this,
            Plasma::ToolTipContent( i18nc( "@info:tooltip", "Current work context: <emphasis>%1</emphasis>",
                                           res.genericLabel() ),
                                    i18nc( "@info:tooltip", "You are working in the context of <emphasis>%1</emphasis>. "
                                           "This means that files, emails, and other resources can easily "
                                           "be related to it.",
                                           res.genericLabel() ),
                                    KIcon( "nepomuk" ) ) );
    }
    else {
        m_button->nativeWidget()->setText( i18nc( "@action:button", "No work context selected" ) );
        Plasma::ToolTipManager::self()->setContent(
            this,
            Plasma::ToolTipContent( i18nc( "@info:tooltip", "No work context selected" ),
                                    i18nc( "@info:tooltip",
                                           "Choose the current work context to be able to easily "
                                           "relate files, emails, and other resources to it." ),
                                    KIcon( "nepomuk" ) ) );
    }
}


void NepomukContextChooser::slotClicked()
{
    m_annotationMenu->exec( QCursor::pos() );
}


void NepomukContextChooser::slotAboutToShow( Nepomuk::AnnotationMenu* menu, bool recreated )
{
    if ( recreated ) {
        QString ginkgoExe = KStandardDirs::findExe( QLatin1String( "ginkgo" ) );
        if ( !ginkgoExe.isEmpty() ) {
            QAction* a = new QAction( menu );
            a->setText( i18n( "Start Ginkgo" ) );
            connect( a, SIGNAL( triggered() ), this, SLOT( slotStartGinkgo() ) );
            menu->addSeparator();
            menu->addAction( a );
        }
    }
}


void NepomukContextChooser::slotStartGinkgo()
{
    QProcess::startDetached( KStandardDirs::findExe( QLatin1String( "ginkgo" ) ) );
}


void NepomukContextChooser::createConfigurationInterface( KConfigDialog* dlg )
{
    m_configWidget = new ConfigWidget( this );
    dlg->addPage( m_configWidget,
                  i18n( "Context Types" ),
                  QLatin1String( "nepomuk" ),
                  i18n( "Select the types that should be selectable as work context" ) );
    connect( dlg, SIGNAL( applyClicked() ), m_configWidget, SLOT( updateAnnotationMenuConfig() ) );
    connect( dlg, SIGNAL( okClicked() ), m_configWidget, SLOT( updateAnnotationMenuConfig() ) );
}


// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(nepomukcontextchooser, NepomukContextChooser)

#include "nepomukcontextchooser.moc"
