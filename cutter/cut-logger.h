/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2012-2013  Kouhei Sutou <kou@clear-code.com>
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

#ifndef __CUT_LOGGER_H__
#define __CUT_LOGGER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define cut_log(level, format, ...)                             \
do {                                                            \
    if (cut_need_log(level)) {                                  \
        (cut_logger_log(cut_logger(),                           \
                        CUT_LOG_DOMAIN,                         \
                        (level),                                \
                        __FILE__,                               \
                        __LINE__,                               \
                        G_STRFUNC,                              \
                        format, ## __VA_ARGS__));               \
    }                                                           \
} while (0)

#define cut_log_critical(format, ...)                           \
    cut_log(CUT_LOG_LEVEL_CRITICAL, format, ## __VA_ARGS__)
#define cut_log_error(format, ...)                              \
    cut_log(CUT_LOG_LEVEL_ERROR, format, ## __VA_ARGS__)
#define cut_log_warning(format, ...)                            \
    cut_log(CUT_LOG_LEVEL_WARNING, format, ## __VA_ARGS__)
#define cut_log_message(format, ...)                            \
    cut_log(CUT_LOG_LEVEL_MESSAGE, format, ## __VA_ARGS__)
#define cut_log_info(format, ...)                       \
    cut_log(CUT_LOG_LEVEL_INFO, format, ## __VA_ARGS__)
#define cut_log_debug(format, ...)                              \
    cut_log(CUT_LOG_LEVEL_DEBUG, format, ## __VA_ARGS__)
#define cut_log_trace(format, ...)                              \
    cut_log(CUT_LOG_LEVEL_TRACE, format, ## __VA_ARGS__)

#define cut_set_log_level(level)                        \
    cut_logger_set_target_level(cut_logger(), (level))
#define cut_get_log_level()                             \
    cut_logger_get_resolved_target_level(cut_logger())
#define cut_get_interesting_log_level()                 \
    cut_logger_get_interesting_level(cut_logger())

#define cut_need_log(level)                     \
    (cut_get_interesting_log_level() & (level))
#define cut_need_critical_log()                 \
    (cut_need_log(CUT_LOG_LEVEL_CRITICAL))
#define cut_need_error_log()                    \
    (cut_need_log(CUT_LOG_LEVEL_ERROR))
#define cut_need_warning_log()                  \
    (cut_need_log(CUT_LOG_LEVEL_WARNING))
#define cut_need_message_log()                  \
    (cut_need_log(CUT_LOG_LEVEL_MESSAGE))
#define cut_need_info_log()                     \
    (cut_need_log(CUT_LOG_LEVEL_INFO))
#define cut_need_debug_log()                    \
    (cut_need_log(CUT_LOG_LEVEL_DEBUG))
#define cut_need_trace_log()                    \
    (cut_need_log(CUT_LOG_LEVEL_TRACE))

#define CUT_TYPE_LOGGER            (cut_logger_get_type())
#define CUT_LOGGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), CUT_TYPE_LOGGER, CutLogger))
#define CUT_LOGGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), CUT_TYPE_LOGGER, CutLoggerClass))
#define CUT_IS_LOGGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), CUT_TYPE_LOGGER))
#define CUT_IS_LOGGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), CUT_TYPE_LOGGER))
#define CUT_LOGGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_LOGGER, CutLoggerClass))

typedef enum
{
    CUT_LOG_LEVEL_DEFAULT    = 0,
    CUT_LOG_LEVEL_NONE       = 1 << 0,
    CUT_LOG_LEVEL_CRITICAL   = 1 << 1,
    CUT_LOG_LEVEL_ERROR      = 1 << 2,
    CUT_LOG_LEVEL_WARNING    = 1 << 3,
    CUT_LOG_LEVEL_MESSAGE    = 1 << 4,
    CUT_LOG_LEVEL_INFO       = 1 << 5,
    CUT_LOG_LEVEL_DEBUG      = 1 << 6,
    CUT_LOG_LEVEL_TRACE      = 1 << 7
} CutLogLevelFlags;

#define CUT_LOG_LEVEL_ALL (CUT_LOG_LEVEL_CRITICAL |          \
                           CUT_LOG_LEVEL_ERROR |             \
                           CUT_LOG_LEVEL_WARNING |           \
                           CUT_LOG_LEVEL_MESSAGE |           \
                           CUT_LOG_LEVEL_INFO |              \
                           CUT_LOG_LEVEL_DEBUG |             \
                           CUT_LOG_LEVEL_TRACE)

#define CUT_LOG_NULL_SAFE_STRING(string) ((string) ? (string) : "(null)")

typedef enum
{
    CUT_LOG_ITEM_DEFAULT     = 0,
    CUT_LOG_ITEM_NONE        = 1 << 0,
    CUT_LOG_ITEM_DOMAIN      = 1 << 1,
    CUT_LOG_ITEM_LEVEL       = 1 << 2,
    CUT_LOG_ITEM_LOCATION    = 1 << 3,
    CUT_LOG_ITEM_TIME        = 1 << 4,
    CUT_LOG_ITEM_NAME        = 1 << 5,
    CUT_LOG_ITEM_PID         = 1 << 6
} CutLogItemFlags;

#define CUT_LOG_ITEM_ALL (CUT_LOG_ITEM_DOMAIN |      \
                          CUT_LOG_ITEM_LEVEL |       \
                          CUT_LOG_ITEM_LOCATION |    \
                          CUT_LOG_ITEM_TIME |        \
                          CUT_LOG_ITEM_NAME |        \
                          CUT_LOG_ITEM_PID)

typedef enum
{
    CUT_LOG_COLORIZE_DEFAULT,
    CUT_LOG_COLORIZE_NONE,
    CUT_LOG_COLORIZE_CONSOLE
} CutLogColorize;

#define CUT_ENUM_ERROR           (cut_enum_error_quark())
#define CUT_FLAGS_ERROR          (cut_flags_error_quark())

typedef enum
{
    CUT_ENUM_ERROR_NULL_NAME,
    CUT_ENUM_ERROR_UNKNOWN_NAME
} CutEnumError;

typedef enum
{
    CUT_FLAGS_ERROR_NULL_NAME,
    CUT_FLAGS_ERROR_UNKNOWN_NAMES
} CutFlagsError;

GQuark           cut_enum_error_quark       (void);
GQuark           cut_flags_error_quark      (void);

typedef struct _CutLogger         CutLogger;
typedef struct _CutLoggerClass    CutLoggerClass;

struct _CutLogger
{
    GObject object;
};

struct _CutLoggerClass
{
    GObjectClass parent_class;

    void (*log) (CutLogger        *logger,
                 const gchar      *domain,
                 CutLogLevelFlags  level,
                 const gchar      *file,
                 guint             line,
                 const gchar      *function,
                 GTimeVal         *time_value,
                 const gchar      *message);
};

GQuark           cut_logger_error_quark    (void);

GType            cut_logger_get_type       (void) G_GNUC_CONST;

CutLogLevelFlags cut_log_level_flags_from_string (const gchar *level_name,
                                                  CutLogLevelFlags base_flags,
                                                  GError     **error);
CutLogItemFlags  cut_log_item_flags_from_string  (const gchar *item_name,
                                                  CutLogItemFlags base_flags,
                                                  GError     **error);

CutLogger       *cut_logger                (void);
void             cut_logger_default_log_handler
                                           (CutLogger        *logger,
                                            const gchar      *domain,
                                            CutLogLevelFlags  level,
                                            const gchar      *file,
                                            guint             line,
                                            const gchar      *function,
                                            GTimeVal         *time_value,
                                            const gchar      *message,
                                            gpointer          user_data);
CutLogger       *cut_logger_new            (void);
void             cut_logger_log            (CutLogger        *logger,
                                            const gchar      *domain,
                                            CutLogLevelFlags  level,
                                            const gchar      *file,
                                            guint             line,
                                            const gchar      *function,
                                            const gchar      *format,
                                            ...) G_GNUC_PRINTF(7, 8);
void             cut_logger_log_va_list    (CutLogger        *logger,
                                            const gchar      *domain,
                                            CutLogLevelFlags  level,
                                            const gchar      *file,
                                            guint             line,
                                            const gchar      *function,
                                            const gchar      *format,
                                            va_list           args);
CutLogLevelFlags cut_logger_get_target_level
                                           (CutLogger        *logger);
CutLogLevelFlags cut_logger_get_resolved_target_level
                                           (CutLogger        *logger);
void             cut_logger_set_target_level
                                           (CutLogger        *logger,
                                            CutLogLevelFlags  level);
gboolean         cut_logger_set_target_level_by_string
                                           (CutLogger        *logger,
                                            const gchar      *level_name,
                                            GError          **error);
void             cut_logger_set_interesting_level
                                           (CutLogger        *logger,
                                            const gchar      *key,
                                            CutLogLevelFlags  level);
CutLogLevelFlags cut_logger_get_interesting_level
                                           (CutLogger        *logger);
CutLogItemFlags  cut_logger_get_target_item(CutLogger        *logger);
void             cut_logger_set_target_item(CutLogger        *logger,
                                            CutLogItemFlags   item);
gboolean         cut_logger_set_target_item_by_string
                                           (CutLogger        *logger,
                                            const gchar      *item_name,
                                            GError          **error);

void             cut_logger_connect_default_handler
                                           (CutLogger        *logger);
void             cut_logger_disconnect_default_handler
                                           (CutLogger        *logger);

#define CUT_GLIB_LOG_DELEGATE(domain)           \
    g_log_set_handler(domain,                   \
                      G_LOG_LEVEL_MASK |        \
                      G_LOG_FLAG_RECURSION |    \
                      G_LOG_FLAG_FATAL,         \
                      cut_glib_log_handler,     \
                      NULL)

void             cut_glib_log_handler      (const gchar         *log_domain,
                                            GLogLevelFlags       log_level,
                                            const gchar         *message,
                                            gpointer             user_data);

G_END_DECLS

#endif /* __CUT_LOGGER_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
