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

#ifndef __CPPCUT_ASSERTIONS_HELPER_H__
#define __CPPCUT_ASSERTIONS_HELPER_H__

#include <string>
#include <sstream>
#include <cutter/cut-helper.h>
#include <cppcutter/cppcut-macros.h>

namespace cut
{
    CPPCUT_DECL
    void assert_equal(int expected, int actual,
                      const char *expression_expected,
                      const char *expression_actual);
    CPPCUT_DECL
    void assert_equal(unsigned int expected, unsigned int actual,
                      const char *expression_expected,
                      const char *expression_actual);
    CPPCUT_DECL
    void assert_equal(long expected, long actual,
                      const char *expression_expected,
                      const char *expression_actual);
    CPPCUT_DECL
    void assert_equal(unsigned long expected, unsigned long actual,
                      const char *expression_expected,
                      const char *expression_actual);
    CPPCUT_DECL
    void assert_equal(long long expected, long long actual,
                      const char *expression_expected,
                      const char *expression_actual);
    CPPCUT_DECL
    void assert_equal(unsigned long long expected, unsigned long long actual,
                      const char *expression_expected,
                      const char *expression_actual);

    template <typename Type> void assert_equal(const Type *expected,
                                               const Type *actual,
                                               const char *expression_expected,
                                               const char *expression_actual)
    {
        assert_equal_reference(expected, actual,
                               expression_expected, expression_actual);
    };

    template <class Type> void assert_equal(const Type& expected,
                                            const Type& actual,
                                            const char *expression_expected,
                                            const char *expression_actual)
    {
        assert_equal_reference(expected, actual,
                               expression_expected, expression_actual);
    };

    template <class Type> void assert_equal_reference(
        const Type& expected, const Type& actual,
        const char *expression_expected, const char *expression_actual)
    {
        if (expected == actual) {
            cut_test_pass();
        } else {
            std::ostringstream inspected_expected;
            std::ostringstream inspected_actual;
            std::ostringstream message;

            inspected_expected << expected;
            cut_set_expected(inspected_expected.str().c_str());

            inspected_actual << actual;
            cut_set_actual(inspected_actual.str().c_str());

            message << "<" << expression_expected << " == ";
            message << expression_actual << ">";
            cut_test_fail(message.str().c_str());
        }
    }

    template <typename Type> void assert_not_null(const Type *object,
                                                  const char *expression_object)
    {
        if (object != NULL) {
            cut_test_pass();
        } else {
            std::ostringstream message;

            message << "expected: <" << object << "> is not NULL";
            cut_test_fail(message.str().c_str());
        }
    }

    CPPCUT_DECL
    void assert_operator(bool result, int lhs, int rhs,
                         const char *expression_lhs,
                         const char *expression_operator,
                         const char *expression_rhs);
    CPPCUT_DECL
    void assert_operator(bool result, unsigned int lhs, unsigned int rhs,
                         const char *expression_lhs,
                         const char *expression_operator,
                         const char *expression_rhs);
    CPPCUT_DECL
    void assert_operator(bool result, long lhs, long rhs,
                         const char *expression_lhs,
                         const char *expression_operator,
                         const char *expression_rhs);
    CPPCUT_DECL
    void assert_operator(bool result, unsigned long lhs, unsigned long rhs,
                         const char *expression_lhs,
                         const char *expression_operator,
                         const char *expression_rhs);
    CPPCUT_DECL
    void assert_operator(bool result, long long lhs, long long rhs,
                         const char *expression_lhs,
                         const char *expression_operator,
                         const char *expression_rhs);
    CPPCUT_DECL
    void assert_operator(bool result,
                         unsigned long long lhs, unsigned long long rhs,
                         const char *expression_lhs,
                         const char *expression_operator,
                         const char *expression_rhs);

    template <typename Type> void assert_operator(
        bool result,
        const Type *lhs, const Type *rhs,
        const char *expression_lhs,
        const char *expression_operator,
        const char *expression_rhs)
    {
        assert_operator_reference(result, lhs, rhs,
                                  expression_lhs,
                                  expression_operator,
                                  expression_rhs);
    }

    template <class Type> void assert_operator(bool result,
                                               const Type& lhs,
                                               const Type& rhs,
                                               const char *expression_lhs,
                                               const char *expression_operator,
                                               const char *expression_rhs)
    {
        assert_operator_reference(result, lhs, rhs,
                                  expression_lhs,
                                  expression_operator,
                                  expression_rhs);
    }

    template <typename Type> void assert_operator_reference(
        bool result,
        const Type& lhs, const Type& rhs,
        const char *expression_lhs,
        const char *expression_operator,
        const char *expression_rhs)
    {
        if (result) {
            cut_test_pass();
        } else {
            std::ostringstream message;

            message << "expected: <" << expression_lhs << "> ";
            message << expression_operator;
            message << " <" << expression_rhs << ">\n";
            message << "  actual: <" << lhs << "> ";
            message << expression_operator;
            message << " <" << rhs << ">";
            cut_test_fail(message.str().c_str());
        }
    }
}

#endif /* __CPPCUT_ASSERTIONS_HELPER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
