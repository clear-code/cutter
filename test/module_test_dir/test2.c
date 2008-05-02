#include <glib.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-module.h>

#define CUT_TYPE_MODULE_TEST2            cut_type_module_test2
#define CUT_MODULE_TEST2(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_MODULE_TEST2, CutModuleTest2))
#define CUT_MODULE_TEST2_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_MODULE_TEST2, CutModuleTest2Class))
#define CUT_IS_MODULE_TEST2(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_MODULE_TEST2))
#define CUT_IS_MODULE_TEST2_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_MODULE_TEST2))
#define CUT_MODULE_TEST2_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_MODULE_TEST2, CutModuleTest2Class))

typedef struct _CutModuleTest2 CutModuleTest2;
typedef struct _CutModuleTest2Class CutModuleTest2Class;

struct _CutModuleTest2
{
    GObject     object;
};

struct _CutModuleTest2Class
{
    GObjectClass parent_class;
};

static GType cut_type_module_test2 = 0;
static GObjectClass *parent_class;

static void
class_init (GObjectClass *klass)
{
    parent_class = g_type_class_peek_parent(klass);
}

static void
init (CutModuleTest2 *test2)
{
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutModuleTest2Class),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutModuleTest2),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_module_test2 =
        g_type_module_register_type(type_module,
                                    G_TYPE_OBJECT,
                                    "CutModuleTest2",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_module_test2)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_module_test2));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_MODULE_TEST2, first_property, var_args);
}

