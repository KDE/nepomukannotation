/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2008 Adrien Bustany <madcat@mymadcat.com>
 *               2008-2010 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "nepomukmenuplugin.h"
#include "annotationmenu.h"
#include "nfo.h"
#include "pimo.h"

#include <konq_popupmenuinformation.h>
#include <KHTMLView>
#include <KHTMLPart>
#include <dom/html_document.h>
#include <dom/dom_string.h>
#include <KTemporaryFile>
#include <KDebug>
#include <KLocalizedString>
#include <KActionMenu>

#include <QtGui/QWidgetAction>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>

#include <Nepomuk/ResourceManager>
#include <Nepomuk/Tag>
#include <Nepomuk/Thing>
#include <Nepomuk/Variant>
#include <Nepomuk/KRatingWidget>


Q_DECLARE_METATYPE( Nepomuk::Tag )

NepomukMenuPlugin::NepomukMenuPlugin( QObject* parent, const QVariantList& )
    : KonqPopupMenuPlugin( parent )
{
    kDebug();
    Nepomuk::ResourceManager::instance()->init();

	m_rateAction = new QWidgetAction( this );
	m_ratingWidget = new KRatingWidget();
	m_rateAction->setDefaultWidget( m_ratingWidget );
	connect( m_ratingWidget, SIGNAL(ratingChanged(unsigned int)),
             this, SLOT(ratingChanged(unsigned int)) );

    m_annotationMenu = new Nepomuk::AnnotationMenu();
    Nepomuk::AnnotationMenu::ConfigurationFlags flags = Nepomuk::AnnotationMenu::ShowAll|Nepomuk::AnnotationMenu::UseSubMenus;
    flags &= ~Nepomuk::AnnotationMenu::ShowRatingAction;
    m_annotationMenu->setConfigurationFlags( flags );
    m_annotationMenu->menuAction()->setText( i18nc("@action:inmenu", "Annotate") );
    connect( m_annotationMenu, SIGNAL( resourcesAnnotated() ),
             this, SLOT( slotResourceAnnotated() ) );
}


NepomukMenuPlugin::~NepomukMenuPlugin()
{
    kDebug();
    delete m_annotationMenu;
}


void NepomukMenuPlugin::setup( KActionCollection* collection, const KonqPopupMenuInformation& info, QMenu* menu )
{
    // We only want 1 item selections
	if( info.items().size() != 1 ) {
        return;
    }

    m_currentUri = info.items().urlList().first();
    if ( m_currentUri.isValid() ) {
        Nepomuk::Resource nfoResource( m_currentUri, Nepomuk::Vocabulary::NFO::Website() );

        // All actions in menu will be deleted by the framework

        // Rating action
        KActionMenu* rateMenu = new KActionMenu( i18nc("@action:inmenu", "Rate this page"), collection );
        rateMenu->addAction(m_rateAction);
        menu->addAction(rateMenu);
        m_ratingWidget->setRating(nfoResource.rating());

        // Annotation sub menu
        m_annotationMenu->setResource( nfoResource );
        menu->addMenu(m_annotationMenu);

        m_view = qobject_cast<KHTMLView*>(info.parentWidget());
    }
}


void NepomukMenuPlugin::slotResourceAnnotated()
{
    saveCurrentPage();
}


Nepomuk::Resource NepomukMenuPlugin::saveCurrentPage()
{
	Nepomuk::Resource nfoResource( m_currentUri, Nepomuk::Vocabulary::NFO::Website() );

	// Try to get the page title by accessing the KHtml component
	if( m_view ) {
        if ( nfoResource.label().isEmpty() ) {
            nfoResource.setLabel( m_view->part()->htmlDocument().title().string() );
        }

        // send the page to Strigi for full text search on annotated webpages
		KTemporaryFile tmpFile;
		tmpFile.setAutoRemove( false );
		tmpFile.open();
		QTextStream s( &tmpFile );
		s << m_view->part()->htmlDocument().toHTML();
		QDBusMessage msg = QDBusMessage::createMethodCall( "org.kde.nepomuk.services.nepomukstrigiservice",
														   "/nepomukstrigiservice",
														   "org.kde.nepomuk.Strigi",
														   "analyzeResourceFromTempFileAndDeleteTempFile" );
		msg.setArguments( QList<QVariant>()
						  << QString::fromAscii( nfoResource.resourceUri().toEncoded() )
						  << QDateTime::currentDateTime().toTime_t()
						  << tmpFile.fileName() );
		QDBusConnection::sessionBus().send( msg );
	}

	return nfoResource;
}


void NepomukMenuPlugin::ratingChanged(unsigned int rating)
{
	saveCurrentPage().setRating(rating);
}

#include "nepomukmenuplugin.moc"
