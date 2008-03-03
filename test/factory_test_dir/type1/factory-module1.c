#include <glib.h>
#include <gmodule.h>

#include <cutter/cut-module-impl.h>
#include <cutter/cut-module-factory.h>
#include <cutter/cut-test.h>

static gchar *function_name = NULL;
static gchar *description = NULL;

#define CUT_TYPE_MODULE_FACTORY_TYPE1_TEST1            cut_type_module_factory_type1_test1
#define CUT_MODULE_FACTORY_TYPE1_TEST1(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_MODULE_FACTORY_TYPE1_TEST1, CutModuleFactoryType1Test1))
#define CUT_MODULE_FACTORY_TYPE1_TEST1_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_MODULE_FACTORY_TYPE1_TEST1, CutModuleFactoryType1Test1Class))
#define CUT_IS_MODULE_FACTORY_TYPE1_TEST1(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_MODULE_FACTORY_TYPE1_TEST1))
#define CUT_IS_MODULE_FACTORY_TYPE1_TEST1_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_MODULE_FACTORY_TYPE1_TEST1))
#define CUT_MODULE_FACTORY_TYPE1_TEST1_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_MODULE_FACTORY_TYPE1_TEST1, CutModuleFactoryType1Test1Class))

typedef struct _CutModuleFactoryType1Test1 CutModuleFactoryType1Test1;
typedef struct _CutModuleFactoryType1Test1Class CutModuleFactoryType1Test1Class;

struct _CutModuleFactoryType1Test1
{
    CutModuleFactory     object;
};

struct _CutModuleFactoryType1Test1Class
{
    CutModuleFactoryClass parent_class;
};

static GType cut_type_module_factory_type1_test1 = 0;
static CutModuleFactoryClass *parent_class;

static void       set_option_group (CutModuleFactory *factory,
                                    GOptionContext   *context);
static GObject   *create           (CutModuleFactory *factory);

static void
class_init (CutModuleFactoryClass *klass)
{
    CutModuleFactoryClass *factory_class;

    parent_class = g_type_class_peek_parent(klass);
    factory_class  = CUT_MODULE_FACTORY_CLASS(klass);

    factory_class->set_option_group = set_option_group;
    factory_class->create           = create;
}

static void
init (CutModuleFactoryType1Test1 *type1_test1)
{
}

static void
register_type (GTypeModule *type_module)
{
    static const GTypeInfo info =
        {
            sizeof (CutModuleFactoryType1Test1Class),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof(CutModuleFactoryType1Test1),
            0,
            (GInstanceInitFunc) init,
        };

    cut_type_module_factory_type1_test1 =
        g_type_module_register_type(type_module,
                                    CUT_TYPE_MODULE_FACTORY,
                                    "CutModuleFactoryType1Test1",
                                    &info, 0);
}

G_MODULE_EXPORT GList *
CUT_MODULE_IMPL_INIT (GTypeModule *type_module)
{
    GList *registered_types = NULL;

    register_type(type_module);
    if (cut_type_module_factory_type1_test1)
        registered_types =
            g_list_prepend(registered_types,
                           (gchar *)g_type_name(cut_type_module_factory_type1_test1));

    return registered_types;
}

G_MODULE_EXPORT void
CUT_MODULE_IMPL_EXIT (void)
{
}

G_MODULE_EXPORT GObject *
CUT_MODULE_IMPL_INSTANTIATE (const gchar *first_property, va_list var_args)
{
    return g_object_new_valist(CUT_TYPE_MODULE_FACTORY_TYPE1_TEST1, first_property, var_args);
}

G_MODULE_EXPORT gchar *
CUT_MODULE_IMPL_GET_LOG_DOMAIN (void)
{
    return g_strdup(G_LOG_DOMAIN);
}

static void
set_option_group (CutModuleFactory *factory, GOptionContext *context)
{
    GOptionGroup *group;
    GOptionEntry entries[] = {
        {"function", 'f', 0, G_OPTION_ARG_STRING, &function_name,
         "Set function name", "FUNCTION_NAME"},
        {"decription", 'd', 0, G_OPTION_ARG_STRING, &description,
         "Set description", "DESCRIPTION"},
        {NULL}
    };

    group = g_option_group_new(("type1-module"),
                               ("Type1 Module Options"),
                               ("TYpe1 Module Options"),
                               factory, NULL);
    g_option_group_add_entries(group, entries);
    g_option_context_add_group(context, group);
}

GObject *
create (CutModuleFactory *factory)
{
    return g_object_new(CUT_TYPE_TEST,
                        "function-name", function_name,
                        "description", description,
                        NULL);
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/

