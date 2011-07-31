/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2008-2009  Kouhei Sutou <kou@cozmixng.org>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __CUT_VERSION_H__
#define __CUT_VERSION_H__

/**
 * SECTION: cut-version
 * @title: Version Information
 * @short_description: Variables and macros to check the Cutter version
 *
 * Cutter provides version information, primarily useful in
 * configure checks for builds that have a configure
 * script. Tests will not typically use the features
 * described here.
 */

/**
 * CUTTER_VERSION_MAJOR:
 *
 * The major version number of the Cutter.
 */
#define CUTTER_VERSION_MAJOR 1

/**
 * CUTTER_VERSION_MINOR:
 *
 * The minor version number of the Cutter.
 */
#define CUTTER_VERSION_MINOR 1

/**
 * CUTTER_VERSION_MICRO:
 *
 * The micro version number of the Cutter.
 */
#define CUTTER_VERSION_MICRO 8

/**
 * CUTTER_VERSION_STRING:
 *
 * The version number string of the Cutter with
 * "#{MAJOR}.#{MINOR}.#{MICRO}" format.
 */
#define CUTTER_VERSION_STRING "1.1.8"

/**
 * CUTTER_CHECK_VERSION:
 * @major: the major version number.
 * @minor: the minor version number.
 * @micro: the micro version number.
 *
 * Checks the version of the Cutter. Returns true if the
 * version of the Cutter header files is the same as or
 * newer than the passed-in version.
 *
 * Since: 1.0.7
 */
#define CUTTER_CHECK_VERSION(major, minor, micro)       \
    (CUTTER_VERSION_MAJOR > (major) ||                  \
     (CUTTER_VERSION_MAJOR == (major) &&                \
      CUTTER_VERSION_MINOR > (minor)) ||                \
     (CUTTER_VERSION_MAJOR == (major) &&                \
      CUTTER_VERSION_MINOR == (minor) &&                \
      CUTTER_VERSION_MICRO >= (micro)))

#endif /* __CUT_VERSION_H__ */

/*
vi:nowrap:ai:expandtab:sw=4
*/
