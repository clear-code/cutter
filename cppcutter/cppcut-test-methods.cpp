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

#include "cppcut-test-methods.h"

#include <cutter.h>

struct LongJumpArguments {
    jmp_buf *jump_buffer;
    gint     value;
};

struct CppCutTestTerminated {
    LongJumpArguments long_jump_arguments;

    CppCutTestTerminated (jmp_buf *jump_buffer, gint value)
    {
        long_jump_arguments.jump_buffer = jump_buffer;
        long_jump_arguments.value       = value;
    }
};

static const gchar *invoking_data_key = "cppcut-test-invoking";

static void
force_unwind_jump_buffers (CutTest *test, jmp_buf *base_jump_buffer)
{
    CutTestContext *test_context = cut_get_current_test_context();
    while (cut_test_context_in_user_message_jump(test_context))
        cut_test_context_finish_user_message_jump(test_context);
    cut_test_context_set_jump_buffer(test_context, base_jump_buffer);
}

void
cut::test::long_jump (CutTestClass  *cut_test_class,
                      CutTest       *test,
                      jmp_buf       *jump_buffer,
                      gint           value)
{
    gpointer invoking_data;
    gboolean *invoking;

    invoking_data = g_object_get_data(G_OBJECT(test), invoking_data_key);
    invoking = static_cast<gboolean *>(invoking_data);
    if (invoking && *invoking &&
        cut_test_is_own_jump_buffer(test, jump_buffer)) {
        throw CppCutTestTerminated(jump_buffer, value);
    } else {
        cut_test_class->long_jump(test, jump_buffer, value);
    }
}

void
cut::test::invoke (CutTestClass *cut_test_class,
                   CutTest *test,
                   CutTestContext *test_context,
                   CutRunContext *run_context)
{
    LongJumpArguments long_jump_arguments;
    bool is_terminated = false;
    const gchar *terminate_message = NULL;
    gboolean invoking = TRUE;
    jmp_buf *base_jump_buffer;

    base_jump_buffer = cut_test_context_get_jump_buffer(test_context);
    g_object_set_data(G_OBJECT(test), invoking_data_key, &invoking);
    try {
        cut_test_class->invoke(test, test_context, run_context);
    } catch (const CppCutTestTerminated &terminated) {
        is_terminated = true;
        long_jump_arguments = terminated.long_jump_arguments;
    } catch (const std::exception &exception) {
        terminate_message =
            cut_take_printf("Unhandled C++ standard exception is thrown: "
                            "<%s>: %s",
                            typeid(exception).name(),
                            exception.what());
    } catch (...) {
        terminate_message = "Unhandled C++ non-standard exception is thrown";
    }
    g_object_steal_data(G_OBJECT(test), invoking_data_key);

    if (is_terminated) {
        cut_test_class->long_jump(test,
                                  long_jump_arguments.jump_buffer,
                                  long_jump_arguments.value);
    }

    if (terminate_message) {
        force_unwind_jump_buffers(test, base_jump_buffer);
        cut_test_terminate(ERROR, terminate_message);
    }
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
