/*
   Copyright (C) 2007-2011 Sebastian Trueg <trueg@kde.org>

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

#ifndef _NEPOMUKANNOTATION_VERSION_H_
#define _NEPOMUKANNOTATION_VERSION_H_

#include "nepomukannotation_export.h"

/// @brief Nepomukannotation version as string at compile time.
#define NEPOMUKANNOTATION_VERSION_STRING "${CMAKE_NEPOMUKANNOTATION_VERSION}"

/// @brief The major Nepomukannotation version number at compile time
#define NEPOMUKANNOTATION_VERSION_MAJOR ${CMAKE_NEPOMUKANNOTATION_VERSION_MAJOR}

/// @brief The minor Nepomukannotation version number at compile time
#define NEPOMUKANNOTATION_VERSION_MINOR ${CMAKE_NEPOMUKANNOTATION_VERSION_MINOR}

/// @brief The Nepomukannotation release version number at compile time
#define NEPOMUKANNOTATION_VERSION_RELEASE ${CMAKE_NEPOMUKANNOTATION_VERSION_RELEASE}

/**
 * \brief Create a unique number from the major, minor and release number of a %Nepomukannotation version
 *
 * This function can be used for preprocessing. For version information at runtime
 * use the version methods in the Nepomukannotation namespace.
 */
#define NEPOMUKANNOTATION_MAKE_VERSION( a,b,c ) (((a) << 16) | ((b) << 8) | (c))

/**
 * \brief %Nepomukannotation Version as a unique number at compile time
 *
 * This macro calculates the %Nepomukannotation version into a number. It is mainly used
 * through NEPOMUKANNOTATION_IS_VERSION in preprocessing. For version information at runtime
 * use the version methods in the Nepomukannotation namespace.
 */
#define NEPOMUKANNOTATION_VERSION \
    NEPOMUKANNOTATION_MAKE_VERSION(NEPOMUKANNOTATION_VERSION_MAJOR,NEPOMUKANNOTATION_VERSION_MINOR,NEPOMUKANNOTATION_VERSION_RELEASE)

/**
 * \brief Check if the %Nepomukannotation version matches a certain version or is higher
 *
 * This macro is typically used to compile conditionally a part of code:
 * \code
 * #if NEPOMUKANNOTATION_IS_VERSION(2,1)
 * // Code for Nepomukannotation 2.1
 * #else
 * // Code for Nepomukannotation 2.0
 * #endif
 * \endcode
 *
 * For version information at runtime
 * use the version methods in the Nepomukannotation namespace.
 */
#define NEPOMUKANNOTATION_IS_VERSION(a,b,c) ( NEPOMUKANNOTATION_VERSION >= NEPOMUKANNOTATION_MAKE_VERSION(a,b,c) )


namespace Nepomuk {
    namespace Annotation {
        /**
         * @brief Returns the major number of Nepomukannotation's version, e.g.
         * 1 for %Nepomukannotation 1.0.2.
         * @return the major version number at runtime.
         */
        NEPOMUKANNOTATION_EXPORT unsigned int versionMajor();

        /**
         * @brief Returns the minor number of Nepomukannotation's version, e.g.
         * 0 for %Nepomukannotation 1.0.2.
         * @return the minor version number at runtime.
         */
        NEPOMUKANNOTATION_EXPORT unsigned int versionMinor();

        /**
         * @brief Returns the release of Nepomukannotation's version, e.g.
         * 2 for %Nepomukannotation 1.0.2.
         * @return the release number at runtime.
         */
        NEPOMUKANNOTATION_EXPORT unsigned int versionRelease();

        /**
         * @brief Returns the %Nepomukannotation version as string, e.g. "1.0.2".
         *
         * On contrary to the macro NEPOMUKANNOTATION_VERSION_STRING this function returns
         * the version number of Nepomukannotation at runtime.
         * @return the %Nepomukannotation version. You can keep the string forever
         */
        NEPOMUKANNOTATION_EXPORT const char* versionString();
    }
}

#endif
