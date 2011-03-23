/*
   This file is part of the Nepomuk KDE project.
   Copyright (C) 2011 Sebastian Trueg <trueg@kde.org>

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

#include "nepomukfileitemplugin.h"
#include "annotationmenu.h"

#include <KTemporaryFile>
#include <KDebug>
#include <KLocalizedString>
#include <KActionMenu>
#include <KFileItemListProperties>
#include <KFileItem>

#include <QtGui/QWidgetAction>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>

#include <Nepomuk/KRatingWidget>


NepomukFileItemPlugin::NepomukFileItemPlugin( QObject* parent, const QVariantList& )
    : KAbstractFileItemActionPlugin( parent )
{
    kDebug();

    m_rateAction = new QWidgetAction( this );
    m_ratingWidget = new KRatingWidget();
    m_rateAction->setDefaultWidget( m_ratingWidget );
    connect( m_ratingWidget, SIGNAL(ratingChanged(int)),
            this, SLOT(ratingChanged(int)) );

    m_annotationMenu = new Nepomuk::AnnotationMenu();
    Nepomuk::AnnotationMenu::ConfigurationFlags flags = Nepomuk::AnnotationMenu::ShowAll|Nepomuk::AnnotationMenu::UseSubMenus;
    flags &= ~Nepomuk::AnnotationMenu::ShowRatingAction;
    m_annotationMenu->setConfigurationFlags( flags );
    m_annotationMenu->menuAction()->setText( i18nc("@action:inmenu", "Annotate") );
}


NepomukFileItemPlugin::~NepomukFileItemPlugin()
{
    kDebug();
    delete m_annotationMenu;
}


QList<QAction *> NepomukFileItemPlugin::actions(const KFileItemListProperties &itemProps, QWidget *parentWidget)
{
    Q_UNUSED(parentWidget);

    // We only want 1 item selections
    if( itemProps.items().size() != 1 ) {
        return QList<QAction*>();
    }

    QList<QAction*> actions;
    m_currentResource = itemProps.items().first().nepomukUri();
    if ( m_currentResource.isValid() ) {
        // Rating action
        KActionMenu* rateMenu = new KActionMenu( i18nc("@action:inmenu", "Personal rating"), this );
        rateMenu->addAction(m_rateAction);
        m_ratingWidget->setRating((int)m_currentResource.rating());
        actions << rateMenu;

        // Annotation sub menu
        m_annotationMenu->setResource(m_currentResource);
        actions << m_annotationMenu->menuAction();
    }

    return actions;
}


void NepomukFileItemPlugin::ratingChanged(int rating)
{
    m_currentResource.setRating(rating);
}

#include "nepomukfileitemplugin.moc"
