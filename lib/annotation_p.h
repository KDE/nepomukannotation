/* This file is part of the Nepomuk Project
   Copyright (c) 2008-2009 Sebastian Trueg <trueg@kde.org>

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

#ifndef _NEPOMUK_ANNOTATION_P_H_
#define _NEPOMUK_ANNOTATION_P_H_

#include <QtGui/QIcon>
#include <QtCore/QString>

namespace Nepomuk {
    class AnnotationPrivate
    {
    public:
        AnnotationPrivate()
            : relevance( 1.0 ) {
        }

        QString label;
        QString comment;
        QIcon icon;
        qreal relevance;
    };
}

#endif
