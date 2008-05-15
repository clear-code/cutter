#include <gcutter.h>
#include <cutter/cut-runner.h>

void test_take_g_object(void);

#define CUT_TYPE_SIMPLE_OBJECT            (cut_simple_object_get_type ())
#define CUT_SIMPLE_OBJECT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_SIMPLE_OBJECT, CutModuleTest1))
#define CUT_SIMPLE_OBJECT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_SIMPLE_OBJECT, CutModuleTest1Class))
#define CUT_IS_SIMPLE_OBJECT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_SIMPLE_OBJECT))
#define CUT_IS_SIMPLE_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_SIMPLE_OBJECT))
#define CUT_SIMPLE_OBJECT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_SIMPLE_OBJECT, CutModuleTest1Class))

typedef struct _CutSimpleObject CutSimpleObject;
typedef struct _CutSimpleObjectClass CutSimpleObjectClass;

struct _CutSimpleObject
{
    GObject     object;
};

struct _CutSimpleObjectClass
{
    GObjectClass parent_class;
};

GType cut_simple_object_get_type (void) G_GNUC_CONST;

G_DEFINE_TYPE(CutSimpleObject, cut_simple_object, G_TYPE_OBJECT)


static void
cut_simple_object_class_init (CutSimpleObjectClass *klass)
{
}

static void
cut_simple_object_init (CutSimpleObject *object)
{
}

static CutTest *test;
static CutRunContext *run_context;
static CutTestContext *test_context;
static CutTestResult *test_result;

static GObject *object;

static gboolean
run (CutTest *_test)
{
    gboolean success;
    CutTestContext *original_test_context;

    test = _test;

    run_context = CUT_RUN_CONTEXT(cut_runner_new());

    test_context = cut_test_context_new(NULL, NULL, test);
    original_test_context = get_current_test_context();
    set_current_test_context(test_context);
    success = cut_test_run(test, test_context, run_context);
    set_current_test_context(original_test_context);

    return success;
}

void
setup (void)
{
    test = NULL;
    run_context = NULL;
    test_context = NULL;
    test_result = NULL;

    object = NULL;
}

void
teardown (void)
{
    if (test)
        g_object_unref(test);
    if (run_context)
        g_object_unref(run_context);
    if (test_context)
        g_object_unref(test_context);
    if (test_result)
        g_object_unref(test_result);
}

static void
take_g_object (void)
{
    cut_take_g_object(object);
}

static void
check_unref (gpointer data, GObject *where_the_object_was)
{
    gboolean *unrefed = data;

    *unrefed = TRUE;
}

void
test_take_g_object (void)
{
    gboolean unrefed = FALSE;

    object = g_object_new(CUT_TYPE_SIMPLE_OBJECT, NULL);
    cut_assert(object);
    g_object_weak_ref(object, check_unref, &unrefed);

    test = cut_test_new("cut_take_g_object test", take_g_object);
    cut_assert(test);

    cut_assert(!unrefed);
    cut_assert(run(test));
    cut_assert(!unrefed);
    g_object_unref(test_context);
    test_context = NULL;
    cut_assert(unrefed);
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
