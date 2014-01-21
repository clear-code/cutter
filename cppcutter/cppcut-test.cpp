/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2014  Kouhei Sutou <kou@clear-code.com>
 *  Copyright (C) 2014  Kazuhiro Yamato <kz0817@gmail.com>
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <exception>
#include <typeinfo>

#include <glib.h>

#include "cppcut-test.h"

#include <cutter.h>

G_BEGIN_DECLS

#define CPPCUT_TEST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CPPCUT_TYPE_TEST, CppCutTestPrivate))
typedef struct _CppCutTestPrivate	CppCutTestPrivate;
struct _CppCutTestPrivate
{
    jmp_buf *jump_buffer_at_invoke;
};

struct LongJumpParam {
    jmp_buf *buf;
    int      val;
};

struct CppCutTermException {
    LongJumpParam param;

    CppCutTermException (jmp_buf *jump_buffer, int val)
    {
        param.buf = jump_buffer;
        param.val = val;
    }
};

G_DEFINE_TYPE(CppCutTest, cppcut_test, CUT_TYPE_TEST)

static void         invoke       (CutTest        *test,
                                  CutTestContext *test_context,
                                  CutRunContext  *run_context);
static void         longjmp_function
                                 (CutTest *test,
                                  jmp_buf *jump_buffer, int val);

static void
cppcut_test_class_init (CppCutTestClass *klass)
{
    GObjectClass *gobject_class;
    CutTestClass *cut_test_class;

    gobject_class = G_OBJECT_CLASS(klass);
    cut_test_class = CUT_TEST_CLASS(klass);

    cut_test_class->invoke = invoke;
    klass->parent_longjmp_function = cut_test_class->longjmp_function;
    cut_test_class->longjmp_function = longjmp_function;

    g_type_class_add_private(gobject_class, sizeof(CppCutTestPrivate)); 
}

static void
cppcut_test_init (CppCutTest *test)
{
    CppCutTestPrivate *priv = CPPCUT_TEST_GET_PRIVATE(test);
    priv->jump_buffer_at_invoke = NULL;
}

CppCutTest *
cppcut_test_new (const gchar *name, CutTestFunction function)
{
    gpointer object;

    object = g_object_new(CPPCUT_TYPE_TEST,
                          "element-name", "test",
                          "name", name,
                          "test-function", function,
                          NULL);
    return CPPCUT_TEST(object);
}

static void
call_parent_longjmp_function (CutTest *test, jmp_buf *jump_buffer, int val)
{
    CppCutTestClass *klass = CPPCUT_TEST_GET_CLASS(test);
    (*klass->parent_longjmp_function)(test, jump_buffer, val);
}

static void
invoke (CutTest *test, CutTestContext *test_context, CutRunContext *run_context)
{
    LongJumpParam long_jump_param;
    bool catch_term_exception = false;
    const gchar *unhandled_exception_msg = NULL;

    CutTestClass *cut_test_class = CUT_TEST_CLASS(cppcut_test_parent_class);
    CppCutTestPrivate *priv = CPPCUT_TEST_GET_PRIVATE(test);
    priv->jump_buffer_at_invoke = cut_test_context_get_jump(test_context);

    try {
        cut_test_class->invoke(test, test_context, run_context);
    } catch (const CppCutTermException &term_exception) {
        catch_term_exception = true;
        long_jump_param = term_exception.param;
    } catch (const std::exception &exception) {
        const char *message;
        message = cut_take_printf("Unhandled C++ standard exception is thrown: "
                                  "<%s>: %s",
                                  typeid(exception).name(),
                                  exception.what());
        unhandled_exception_msg = message;
    } catch (...) {
        unhandled_exception_msg = "Unhandled C++ non-standard exception is thrown";
    }

    priv->jump_buffer_at_invoke = NULL;

    if (catch_term_exception) {
        call_parent_longjmp_function(test, long_jump_param.buf,
                                     long_jump_param.val);
    }

    // cut_test_terminate() must be called outside of the above
    // try-catch block. Or an exception instance is not destroyed.
    if (unhandled_exception_msg)
        cut_test_terminate(ERROR, unhandled_exception_msg);
}

static void
longjmp_function (CutTest *test, jmp_buf *jump_buffer, int val)
{
    CppCutTestPrivate *priv = CPPCUT_TEST_GET_PRIVATE(test);
    if (jump_buffer == priv->jump_buffer_at_invoke)
        throw CppCutTermException(jump_buffer, val);
    else
        call_parent_longjmp_function(test, jump_buffer, val);
}

G_END_DECLS

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
