/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2011  Kouhei Sutou <kou@clear-code.com>
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

#include <typeinfo>
#include "cppcut-assertions-helper.h"

CPPCUT_DECL void
cut::inspect_c_string(std::ostream &stream,
                      const char *c_string, unsigned int length)
{
    stream << '"';
    for (unsigned int i = 0; i < length; i++) {
        switch (c_string[i]) {
        case '"':
            stream << "\\\"";
            break;
        case '\\':
            stream << "\\\\";
            break;
        default:
            stream << c_string[i];
            break;
        }
    }
    stream << '"';

}

CPPCUT_DECL void
cut::assert_equal(char *expected, char *actual,
                  const char *expression_expected,
                  const char *expression_actual)
{
    cut::assert_equal(static_cast<const char *>(expected),
                      static_cast<const char *>(actual),
                      expression_expected, expression_actual);
}

CPPCUT_DECL void
cut::assert_equal(const char *expected, const char *actual,
                  const char *expression_expected,
                  const char *expression_actual)
{
    cut_assert_equal_string_helper(expected, actual,
                                   expression_expected, expression_actual);
}

template <>
CPPCUT_DECL void
cut::assert_equal_reference<std::type_info>(const std::type_info& expected,
                                            const std::type_info& actual,
                                            const char *expression_expected,
                                            const char *expression_actual)
{
    if (expected == actual) {
        cut_test_pass();
    } else {
        std::ostringstream message;

        cut_set_expected(expected.name());
        cut_set_actual(actual.name());

        message << "<" << expression_expected << " == ";
        message << expression_actual << ">";
        cut_test_fail(message.str().c_str());
    }
}

CPPCUT_DECL void
cut::assert_not_equal(char *expected, char *actual,
                      const char *expression_expected,
                      const char *expression_actual)
{
    cut::assert_not_equal(static_cast<const char *>(expected),
                          static_cast<const char *>(actual),
                          expression_expected,
                          expression_actual);
}

CPPCUT_DECL void
cut::assert_not_equal(const char *expected, const char *actual,
                      const char *expression_expected,
                      const char *expression_actual)
{
    cut_assert_not_equal_string_helper(expected, actual,
                                       expression_expected,
                                       expression_actual);
}

CPPCUT_DECL void
cut::assert_operator(bool result, int lhs, int rhs,
                     const char *expression_lhs,
                     const char *expression_operator,
                     const char *expression_rhs)
{
    cut::assert_operator_reference(result,
                                   lhs, rhs,
                                   expression_lhs,
                                   expression_operator,
                                   expression_rhs);
}

CPPCUT_DECL void
cut::assert_operator(bool result, unsigned int lhs, unsigned int rhs,
                     const char *expression_lhs,
                     const char *expression_operator,
                     const char *expression_rhs)
{
    cut::assert_operator_reference(result,
                                   lhs, rhs,
                                   expression_lhs,
                                   expression_operator,
                                   expression_rhs);
}

CPPCUT_DECL void
cut::assert_operator(bool result, long lhs, long rhs,
                     const char *expression_lhs,
                     const char *expression_operator,
                     const char *expression_rhs)
{
    cut::assert_operator_reference(result,
                                   lhs, rhs,
                                   expression_lhs,
                                   expression_operator,
                                   expression_rhs);
}

CPPCUT_DECL void
cut::assert_operator(bool result, unsigned long lhs, unsigned long rhs,
                     const char *expression_lhs,
                     const char *expression_operator,
                     const char *expression_rhs)
{
    cut::assert_operator_reference(result,
                                   lhs, rhs,
                                   expression_lhs,
                                   expression_operator,
                                   expression_rhs);
}

CPPCUT_DECL void
cut::assert_operator(bool result, long long lhs, long long rhs,
                     const char *expression_lhs,
                     const char *expression_operator,
                     const char *expression_rhs)
{
    cut::assert_operator_reference(result,
                                   lhs, rhs,
                                   expression_lhs,
                                   expression_operator,
                                   expression_rhs);
}

CPPCUT_DECL void
cut::assert_operator(bool result,
                     unsigned long long lhs, unsigned long long rhs,
                     const char *expression_lhs,
                     const char *expression_operator,
                     const char *expression_rhs)
{
    cut::assert_operator_reference(result,
                                   lhs, rhs,
                                   expression_lhs,
                                   expression_operator,
                                   expression_rhs);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
