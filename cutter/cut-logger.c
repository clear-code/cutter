/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2012  Kouhei Sutou <kou@clear-code.com>
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
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include <glib.h>

#include "cut-logger.h"
#include "cut-enum-types.h"
#include "../gcutter/gcut-marshalers.h"

#define DEFAULT_KEY "default"
#define DEFAULT_LEVEL                        \
    (CUT_LOG_LEVEL_CRITICAL |                \
     CUT_LOG_LEVEL_ERROR |                   \
     CUT_LOG_LEVEL_WARNING |                 \
     CUT_LOG_LEVEL_MESSAGE |                 \
     CUT_LOG_LEVEL_STATISTICS)
#define DEFAULT_ITEM                            \
    (CUT_LOG_ITEM_TIME)

#define CUT_LOGGER_GET_PRIVATE(obj)                     \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj),                 \
                                 CUT_TYPE_LOGGER,       \
                                 CutLoggerPrivate))

#define INTERNAL_INTERESTING_LOG_LEVEL                          \
    (cut_logger_get_interesting_level(singleton_cut_logger))

#define INTERNAL_LOG(level, format, ...) do {           \
    if (INTERNAL_INTERESTING_LOG_LEVEL & (level)) {     \
        cut_logger_log(singleton_cut_logger,            \
                       CUT_LOG_DOMAIN,                  \
                       (level),                         \
                       __FILE__,                        \
                       __LINE__,                        \
                       G_STRFUNC,                       \
                       format, ## __VA_ARGS__);         \
    }                                                   \
} while (0)


typedef struct _CutLoggerPrivate	CutLoggerPrivate;
struct _CutLoggerPrivate
{
    CutLogLevelFlags target_level;
    CutLogItemFlags target_item;
    GHashTable *interesting_levels;
    CutLogLevelFlags interesting_level;
};

enum
{
    PROP_0,
    PROP_TARGET_LEVEL,
    PROP_TARGET_ITEM,
    PROP_INTERESTING_LEVEL
};

enum
{
    LOG,
    LAST_SIGNAL
};

static gint signals[LAST_SIGNAL] = {0};

static CutLogger *singleton_cut_logger = NULL;

G_DEFINE_TYPE(CutLogger, cut_logger, G_TYPE_OBJECT);

static void dispose        (GObject         *object);
static void set_property   (GObject         *object,
                            guint            prop_id,
                            const GValue    *value,
                            GParamSpec      *pspec);
static void get_property   (GObject         *object,
                            guint            prop_id,
                            GValue          *value,
                            GParamSpec      *pspec);

static void
cut_logger_class_init (CutLoggerClass *klass)
{
    GObjectClass *gobject_class;
    GParamSpec *spec;

    gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose      = dispose;
    gobject_class->set_property = set_property;
    gobject_class->get_property = get_property;

    spec = g_param_spec_flags("target-level",
                              "Target log level",
                              "The target log level",
                              CUT_TYPE_LOG_LEVEL_FLAGS,
                              CUT_LOG_LEVEL_DEFAULT,
                              G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TARGET_LEVEL, spec);

    spec = g_param_spec_flags("target-item",
                              "Target log item",
                              "The target log item",
                              CUT_TYPE_LOG_ITEM_FLAGS,
                              CUT_LOG_ITEM_DEFAULT,
                              G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_TARGET_ITEM, spec);

    spec = g_param_spec_flags("interesting-level",
                              "Interesting log level",
                              "The interesting log level",
                              CUT_TYPE_LOG_LEVEL_FLAGS,
                              DEFAULT_LEVEL,
                              G_PARAM_READABLE);
    g_object_class_install_property(gobject_class, PROP_INTERESTING_LEVEL, spec);

    signals[LOG] =
        g_signal_new("log",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(CutLoggerClass, log),
                     NULL, NULL,
                     _gcut_marshal_VOID__STRING_FLAGS_STRING_UINT_STRING_POINTER_STRING,
                     G_TYPE_NONE, 7,
                     G_TYPE_STRING, CUT_TYPE_LOG_LEVEL_FLAGS, G_TYPE_STRING,
                     G_TYPE_UINT, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_STRING);

    g_type_class_add_private(gobject_class, sizeof(CutLoggerPrivate));
}

static void
cut_logger_init (CutLogger *logger)
{
    CutLoggerPrivate *priv;

    priv = CUT_LOGGER_GET_PRIVATE(logger);
    priv->target_level = CUT_LOG_LEVEL_DEFAULT;
    priv->interesting_levels = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                     g_free, NULL);
    priv->interesting_level = DEFAULT_LEVEL;
    g_hash_table_insert(priv->interesting_levels,
                        g_strdup(DEFAULT_KEY),
                        GUINT_TO_POINTER(priv->interesting_level));
}

static void
dispose (GObject *object)
{
    CutLoggerPrivate *priv;

    priv = CUT_LOGGER_GET_PRIVATE(object);

    if (priv->interesting_levels) {
        g_hash_table_unref(priv->interesting_levels);
        priv->interesting_levels = NULL;
    }

    G_OBJECT_CLASS(cut_logger_parent_class)->dispose(object);
}

static void
set_property (GObject      *object,
              guint         prop_id,
              const GValue *value,
              GParamSpec   *pspec)
{
    CutLoggerPrivate *priv;

    priv = CUT_LOGGER_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_TARGET_LEVEL:
        priv->target_level = g_value_get_flags(value);
        break;
    case PROP_TARGET_ITEM:
        priv->target_item = g_value_get_flags(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
get_property (GObject    *object,
              guint       prop_id,
              GValue     *value,
              GParamSpec *pspec)
{
    CutLoggerPrivate *priv;

    priv = CUT_LOGGER_GET_PRIVATE(object);
    switch (prop_id) {
    case PROP_TARGET_LEVEL:
        g_value_set_flags(value, priv->target_level);
        break;
    case PROP_TARGET_ITEM:
        g_value_set_flags(value, priv->target_item);
        break;
    case PROP_INTERESTING_LEVEL:
        g_value_set_flags(value, priv->interesting_level);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

GQuark
cut_flags_error_quark (void)
{
    return g_quark_from_static_string("cut-flags-error-quark");
}

GQuark
cut_enum_error_quark (void)
{
    return g_quark_from_static_string("cut-enum-error-quark");
}

static guint
flags_from_string (GType        flags_type,
                   const gchar *flags_string,
                   guint        base_flags,
                   GError     **error)
{
    gchar **split_names, **names;
    GFlagsClass *flags_class;
    guint flags = 0;
    gboolean append = TRUE;
    GString *unknown_strings = NULL;

    if (!flags_string) {
        g_set_error(error,
                    CUT_FLAGS_ERROR,
                    CUT_FLAGS_ERROR_NULL_NAME,
                    "flags name is NULL (<%s>)",
                    g_type_name(flags_type));
        return 0;
    }

    switch (flags_string[0]) {
    case '+':
        flags = base_flags;
        flags_string++;
        break;
    case '-':
        append = FALSE;
        flags = base_flags;
        flags_string++;
        break;
    default:
        break;
    }

    split_names = g_strsplit(flags_string, "|", 0);
    flags_class = g_type_class_ref(flags_type);
    for (names = split_names; *names; names++) {
        gchar *name = *names;
        gboolean local_append = append;

        switch (name[0]) {
        case '+':
            local_append = TRUE;
            name++;
            break;
        case '-':
            local_append = FALSE;
            name++;
            break;
        default:
            break;
        }

        if (g_str_equal(name, "all")) {
            if (local_append) {
                flags |= flags_class->mask;
            } else {
                flags = 0;
            }
        } else {
            GFlagsValue *value;

            value = g_flags_get_value_by_nick(flags_class, name);
            if (value) {
                if (local_append) {
                    flags |= value->value;
                } else {
                    flags &= ~(value->value);
                }
            } else {
                if (!unknown_strings)
                    unknown_strings = g_string_new(NULL);
                g_string_append_printf(unknown_strings,
                                       "%s<%s>",
                                       unknown_strings->len > 0 ? "|" : "",
                                       name);
            }
        }
    }
    g_type_class_unref(flags_class);
    g_strfreev(split_names);

    if (unknown_strings) {
        g_set_error(error,
                    CUT_FLAGS_ERROR,
                    CUT_FLAGS_ERROR_UNKNOWN_NAMES,
                    "unknown flag names: [%s](<%s>)",
                    unknown_strings->str,
                    g_type_name(flags_type));
        g_string_free(unknown_strings, TRUE);
    }

    return flags;
}

CutLogLevelFlags
cut_log_level_flags_from_string (const gchar *level_name,
                                 CutLogLevelFlags base_flags,
                                 GError **error)
{
    if (g_str_equal(level_name, "all")) {
        return CUT_LOG_LEVEL_ALL;
    } else {
        if (base_flags == CUT_LOG_LEVEL_DEFAULT &&
            level_name &&
            (level_name[0] == '+' || level_name[0] == '-')) {
            base_flags = DEFAULT_LEVEL;
        }
        return flags_from_string(CUT_TYPE_LOG_LEVEL_FLAGS,
                                 level_name,
                                 base_flags,
                                 error);
    }
}

static gint
enum_from_string (GType        enum_type,
                  const gchar *enum_string,
                  GError     **error)
{
    GEnumClass *enum_class;
    GEnumValue *enum_value;
    gint value = 0;

    if (!enum_string) {
        g_set_error(error,
                    CUT_ENUM_ERROR,
                    CUT_ENUM_ERROR_NULL_NAME,
                    "enum name is NULL (<%s>)",
                    g_type_name(enum_type));
        return 0;
    }

    enum_class = g_type_class_ref(enum_type);
    enum_value = g_enum_get_value_by_nick(enum_class, enum_string);
    if (enum_value) {
        value = enum_value->value;
    } else {
        g_set_error(error,
                    CUT_ENUM_ERROR,
                    CUT_ENUM_ERROR_UNKNOWN_NAME,
                    "unknown enum name: <%s>(<%s>)",
                    enum_string,
                    g_type_name(enum_type));
    }
    g_type_class_unref(enum_class);

    return value;
}

CutLogItemFlags
cut_log_item_flags_from_string (const gchar *item_name,
                                   CutLogItemFlags base_flags,
                                   GError **error)
{
    if (base_flags == CUT_LOG_ITEM_DEFAULT &&
        item_name &&
        (item_name[0] == '+' || item_name[0] == '-')) {
        base_flags = DEFAULT_ITEM;
    }
    return flags_from_string(CUT_TYPE_LOG_ITEM_FLAGS,
                             item_name,
                             base_flags,
                             error);
}

#define BLACK_COLOR "\033[01;30m"
#define BLACK_BACK_COLOR "\033[40m"
#define RED_COLOR "\033[01;31m"
#define RED_BACK_COLOR "\033[41m"
#define GREEN_COLOR "\033[01;32m"
#define GREEN_BACK_COLOR "\033[01;42m"
#define YELLOW_COLOR "\033[01;33m"
#define YELLOW_BACK_COLOR "\033[01;43m"
#define BLUE_COLOR "\033[01;34m"
#define BLUE_BACK_COLOR "\033[01;44m"
#define MAGENTA_COLOR "\033[01;35m"
#define MAGENTA_BACK_COLOR "\033[01;45m"
#define CYAN_COLOR "\033[01;36m"
#define CYAN_BACK_COLOR "\033[01;46m"
#define WHITE_COLOR "\033[01;37m"
#define WHITE_BACK_COLOR "\033[01;47m"
#define NORMAL_COLOR "\033[00m"

static void
log_message_colorize_console (GString *log,
                              CutLogLevelFlags level,
                              const gchar *message)
{
    const gchar *color = NULL;

    switch (level) {
    case CUT_LOG_LEVEL_ERROR:
        color = WHITE_COLOR RED_BACK_COLOR;
        break;
    case CUT_LOG_LEVEL_CRITICAL:
        color = YELLOW_COLOR RED_BACK_COLOR;
        break;
    case CUT_LOG_LEVEL_WARNING:
        color = WHITE_COLOR YELLOW_BACK_COLOR;
        break;
    case CUT_LOG_LEVEL_MESSAGE:
        color = WHITE_COLOR GREEN_BACK_COLOR;
        break;
    case CUT_LOG_LEVEL_INFO:
        color = WHITE_COLOR CYAN_BACK_COLOR;
        break;
    case CUT_LOG_LEVEL_DEBUG:
        color = WHITE_COLOR BLUE_BACK_COLOR;
        break;
    case CUT_LOG_LEVEL_TRACE:
        color = WHITE_COLOR MAGENTA_BACK_COLOR;
        break;
    case CUT_LOG_LEVEL_STATISTICS:
        color = BLUE_COLOR WHITE_BACK_COLOR;
        break;
    case CUT_LOG_LEVEL_PROFILE:
        color = GREEN_COLOR BLACK_BACK_COLOR;
        break;
    default:
        color = NULL;
        break;
    }

    if (color)
        g_string_append_printf(log, "%s%s%s", color, message, NORMAL_COLOR);
    else
        g_string_append(log, message);
}

static gboolean
guess_console_color_usability (void)
{
    const gchar *term;

    term = g_getenv("TERM");
    if (term && (g_str_has_suffix(term, "term") ||
                 g_str_has_suffix(term, "term-color") ||
                 g_str_equal(term, "screen") ||
                 g_str_equal(term, "linux")))
        return TRUE;

    return FALSE;
}

static void
log_message (GString *log, CutLogLevelFlags level, const gchar *message)
{
    const gchar *colorize_type;
    CutLogColorize colorize = CUT_LOG_COLORIZE_DEFAULT;

    colorize_type = g_getenv("CUT_LOG_COLORIZE");
    if (colorize_type)
        colorize = enum_from_string(CUT_TYPE_LOG_COLORIZE,
                                    colorize_type,
                                    NULL);

    if (colorize == CUT_LOG_COLORIZE_DEFAULT) {

        if (isatty(STDOUT_FILENO) && guess_console_color_usability()) {
            colorize = CUT_LOG_COLORIZE_CONSOLE;
        } else {
            colorize = CUT_LOG_COLORIZE_NONE;
        }
    }

    switch (colorize) {
    case CUT_LOG_COLORIZE_CONSOLE:
        log_message_colorize_console(log, level, message);
        break;
    default:
        g_string_append(log, message);
        break;
    }
}

static inline void
check_cut_debug (CutLogLevelFlags level)
{
    if (CUT_LOG_LEVEL_CRITICAL <= level &&
        level <= CUT_LOG_LEVEL_WARNING) {
        const gchar *cut_debug;

        cut_debug = g_getenv("CUT_DEBUG");
        if (cut_debug &&
            (strcmp(cut_debug, "fatal-warnings") == 0 ||
             strcmp(cut_debug, "fatal_warnings") == 0)) {
            abort();
        }
    }
}

void
cut_logger_default_log_handler (CutLogger *logger, const gchar *domain,
                                   CutLogLevelFlags level,
                                   const gchar *file, guint line,
                                   const gchar *function,
                                   GTimeVal *time_value, const gchar *message,
                                   gpointer user_data)
{
    CutLoggerPrivate *priv;
    CutLogLevelFlags target_level;
    CutLogItemFlags target_item;
    GString *log;

    priv = CUT_LOGGER_GET_PRIVATE(logger);
    target_level = cut_logger_get_resolved_target_level(logger);
    if (!(level & target_level))
        return;

    check_cut_debug(level);

    log = g_string_new(NULL);

    target_item = priv->target_item;
    if (target_item == CUT_LOG_ITEM_DEFAULT)
        target_item = DEFAULT_ITEM;

    if (target_item & CUT_LOG_ITEM_LEVEL) {
        GFlagsClass *flags_class;

        flags_class = g_type_class_ref(CUT_TYPE_LOG_LEVEL_FLAGS);
        if (flags_class) {
            if (level & flags_class->mask) {
                guint i;
                for (i = 0; i < flags_class->n_values; i++) {
                    GFlagsValue *value = flags_class->values + i;
                    if (level & value->value)
                        g_string_append_printf(log, "[%s]", value->value_nick);
                }
            }
            g_type_class_unref(flags_class);
        }
    }

    if (domain && (target_item & CUT_LOG_ITEM_DOMAIN))
        g_string_append_printf(log, "[%s]", domain);

    if (target_item & CUT_LOG_ITEM_TIME) {
        gchar *time_string;

        time_string = g_time_val_to_iso8601(time_value);
        g_string_append_printf(log, "[%s]", time_string);
        g_free(time_string);
    }

    if (target_item & CUT_LOG_ITEM_NAME) {
        if (log->len > 0)
            g_string_append(log, " ");
        g_string_append_printf(log, "[%s]", g_get_prgname());
    }

    if (target_item & CUT_LOG_ITEM_PID) {
        if (log->len > 0 && !(target_item & CUT_LOG_ITEM_NAME))
            g_string_append(log, " ");
        g_string_append_printf(log, "[%u]", getpid());
    }

    if (file && (target_item & CUT_LOG_ITEM_LOCATION)) {
        if (log->len > 0)
            g_string_append(log, " ");
        g_string_append_printf(log, "%s:%d: ", file, line);
        if (function)
            g_string_append_printf(log, "%s(): ", function);
    } else {
        if (log->len > 0)
            g_string_append(log, ": ");
    }

    log_message(log, level, message);
    g_string_append(log, "\n");
    g_print("%s", log->str);
    g_string_free(log, TRUE);
}

CutLogger *
cut_logger (void)
{
    if (!singleton_cut_logger) {
        GError *error = NULL;

        singleton_cut_logger = cut_logger_new();
        cut_logger_connect_default_handler(singleton_cut_logger);
        if (!cut_logger_set_target_level_by_string(singleton_cut_logger,
                                                      g_getenv("CUT_LOG_LEVEL"),
                                                      &error)) {
            INTERNAL_LOG(CUT_LOG_LEVEL_WARNING,
                         "[logger][level][set][warning] %s", error->message);
            g_error_free(error);
            error = NULL;
        }
        if (!cut_logger_set_target_item_by_string(singleton_cut_logger,
                                                     g_getenv("CUT_LOG_ITEM"),
                                                     &error)) {
            INTERNAL_LOG(CUT_LOG_LEVEL_WARNING,
                         "[logger][item][set][warning] %s", error->message);
            g_error_free(error);
            error = NULL;
        }
    }

    return singleton_cut_logger;
}

CutLogger *
cut_logger_new (void)
{
    return g_object_new(CUT_TYPE_LOGGER,
                        NULL);
}

void
cut_logger_log (CutLogger *logger,
                const gchar *domain, CutLogLevelFlags level,
                const gchar *file, guint line, const gchar *function,
                const gchar *format, ...)
{
    va_list args;

    va_start(args, format);
    cut_logger_log_va_list(logger, domain, level, file, line, function,
                           format, args);
    va_end(args);
}

void
cut_logger_log_va_list (CutLogger *logger,
                        const gchar *domain, CutLogLevelFlags level,
                        const gchar *file, guint line, const gchar *function,
                        const gchar *format, va_list args)
{
    GTimeVal time_value;
    gchar *message;

    g_get_current_time(&time_value);

    message = g_strdup_vprintf(format, args);
    g_signal_emit(logger, signals[LOG], 0,
                  domain, level, file, line, function, &time_value, message);
    g_free(message);
}

CutLogLevelFlags
cut_logger_get_target_level (CutLogger *logger)
{
    return CUT_LOGGER_GET_PRIVATE(logger)->target_level;
}

CutLogLevelFlags
cut_logger_get_resolved_target_level (CutLogger *logger)
{
    CutLogLevelFlags target_level;

    target_level = CUT_LOGGER_GET_PRIVATE(logger)->target_level;
    if (target_level == CUT_LOG_LEVEL_DEFAULT)
        target_level = DEFAULT_LEVEL;
    return target_level;
}

void
cut_logger_set_target_level (CutLogger *logger,
                             CutLogLevelFlags level)
{
    CutLogLevelFlags interesting_level = level;

    CUT_LOGGER_GET_PRIVATE(logger)->target_level = level;
    if (interesting_level == CUT_LOG_LEVEL_DEFAULT)
        interesting_level = DEFAULT_LEVEL;
    cut_logger_set_interesting_level(logger, DEFAULT_KEY, interesting_level);
}

gboolean
cut_logger_set_target_level_by_string (CutLogger *logger,
                                       const gchar *level_name,
                                       GError **error)
{
    CutLogLevelFlags level;

    if (level_name) {
        GError *local_error = NULL;
        CutLogLevelFlags current_level;
        current_level = cut_logger_get_target_level(logger);
        level = cut_log_level_flags_from_string(level_name,
                                                current_level,
                                                &local_error);
        if (local_error) {
            g_propagate_error(error, local_error);
            return FALSE;
        }
    } else {
        level = CUT_LOG_LEVEL_DEFAULT;
    }
    cut_logger_set_target_level(logger, level);
    return TRUE;
}

static void
update_interesting_level (gpointer key, gpointer value, gpointer user_data)
{
    CutLogLevelFlags *level = user_data;

    *level |= GPOINTER_TO_UINT(value);
}

void
cut_logger_set_interesting_level (CutLogger *logger,
                                  const gchar *name,
                                  CutLogLevelFlags level)
{
    CutLoggerPrivate *priv;

    priv = CUT_LOGGER_GET_PRIVATE(logger);

    g_hash_table_insert(priv->interesting_levels,
                        g_strdup(name),
                        GUINT_TO_POINTER(level));
    priv->interesting_level = 0;
    g_hash_table_foreach(priv->interesting_levels,
                         update_interesting_level,
                         &(priv->interesting_level));
}

CutLogLevelFlags
cut_logger_get_interesting_level (CutLogger *logger)
{
    return CUT_LOGGER_GET_PRIVATE(logger)->interesting_level;
}

CutLogItemFlags
cut_logger_get_target_item (CutLogger *logger)
{
    return CUT_LOGGER_GET_PRIVATE(logger)->target_item;
}


void
cut_logger_set_target_item (CutLogger *logger,
                            CutLogItemFlags item)
{
    CUT_LOGGER_GET_PRIVATE(logger)->target_item = item;
}

gboolean
cut_logger_set_target_item_by_string (CutLogger *logger,
                                      const gchar *item_name,
                                      GError **error)
{
    CutLogItemFlags item;

    if (item_name) {
        GError *local_error = NULL;
        CutLogItemFlags current_item;
        current_item = cut_logger_get_target_item(logger);
        item = cut_log_item_flags_from_string(item_name,
                                              current_item,
                                              &local_error);
        if (local_error) {
            g_propagate_error(error, local_error);
            return FALSE;
        }
    } else {
        item = CUT_LOG_ITEM_DEFAULT;
    }
    cut_logger_set_target_item(logger, item);
    return TRUE;
}

void
cut_logger_connect_default_handler (CutLogger *logger)
{
    g_signal_connect(logger, "log",
                     G_CALLBACK(cut_logger_default_log_handler), NULL);
}

void
cut_logger_disconnect_default_handler (CutLogger *logger)
{
    g_signal_handlers_disconnect_by_func(
        logger, G_CALLBACK(cut_logger_default_log_handler), NULL);
}

void
cut_glib_log_handler (const gchar         *log_domain,
                      GLogLevelFlags       log_level,
                      const gchar         *message,
                      gpointer             user_data)
{
    CutLogger *logger = user_data;

    if (!logger)
        logger = cut_logger();

#define LOG(level)                                              \
    cut_logger_log(logger, "glib-log",                          \
                   CUT_LOG_LEVEL_ ## level,                     \
                   NULL, 0, NULL,                               \
                   "%s%s%s %s",                                 \
                   log_domain ? "[" : "",                       \
                   log_domain ? log_domain : "",                \
                   log_domain ? "]" : "",                       \
                   message)

    switch (log_level & G_LOG_LEVEL_MASK) {
    case G_LOG_LEVEL_ERROR:
        LOG(ERROR);
        break;
    case G_LOG_LEVEL_CRITICAL:
        LOG(CRITICAL);
        break;
    case G_LOG_LEVEL_WARNING:
        LOG(WARNING);
        break;
    case G_LOG_LEVEL_MESSAGE:
        LOG(MESSAGE);
        break;
    case G_LOG_LEVEL_INFO:
        LOG(INFO);
        break;
    case G_LOG_LEVEL_DEBUG:
        LOG(DEBUG);
        break;
    default:
        break;
    }
#undef LOG
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
