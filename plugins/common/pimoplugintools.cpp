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

#include "pimoplugintools.h"

qreal calculateDistance( const QString& s1, const QString& s2 )
{
    // FIXME: use some smarter distance
    if ( s1.compare( s2, Qt::CaseInsensitive ) == 0 ) {
        return 0.0;
    }
    else {
        int diff = qAbs( s1.length() - s2.length() );
        int len = qMax( s1.length(), s2.length() );
        qreal d = ( qreal )diff/( qreal )len;

        int pos = s1.indexOf( s2, 0, Qt::CaseInsensitive );
        if ( pos == 0 ) {
            // better match
            d *= 0.7;
        }
        return d;
    }
}
