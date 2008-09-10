#include <gcutter.h>
#include <cutter/cut-run-context.h>

void test_max_threads(void);

#define CUT_TYPE_RUN_CONTEXT_STUB            (cut_run_context_stub_get_type ())
#define CUT_RUN_CONTEXT_STUB(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CUT_TYPE_RUN_CONTEXT_STUB, CutRunContextStub))
#define CUT_RUN_CONTEXT_STUB_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CUT_TYPE_RUN_CONTEXT_STUB, CutRunContextStubClass))
#define CUT_IS_RUN_CONTEXT_STUB(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CUT_TYPE_RUN_CONTEXT_STUB))
#define CUT_IS_RUN_CONTEXT_STUB_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CUT_TYPE_RUN_CONTEXT_STUB))
#define CUT_RUN_CONTEXT_STUB_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), CUT_TYPE_RUN_CONTEXT_STUB, CutRunContextStubClass))

typedef struct _CutRunContextStub CutRunContextStub;
typedef struct _CutRunContextStubClass CutRunContextStubClass;

struct _CutRunContextStub
{
    CutRunContext     object;
};

struct _CutRunContextStubClass
{
    CutRunContextClass parent_class;
};

GType cut_run_context_stub_get_type (void) G_GNUC_CONST;

G_DEFINE_TYPE(CutRunContextStub, cut_run_context_stub, CUT_TYPE_RUN_CONTEXT)


static void
cut_run_context_stub_class_init (CutRunContextStubClass *klass)
{
}

static void
cut_run_context_stub_init (CutRunContextStub *object)
{
}

static CutRunContext *run_context;

void
setup (void)
{
    run_context = NULL;
}

void
teardown (void)
{
    if (run_context)
        g_object_unref(run_context);
}

void
test_max_threads (void)
{
    run_context = g_object_new(CUT_TYPE_RUN_CONTEXT_STUB, NULL);
    cut_assert(run_context);

    cut_assert_equal_int(10, cut_run_context_get_max_threads(run_context));

    cut_run_context_set_max_threads(run_context, 100);
    cut_assert_equal_int(100, cut_run_context_get_max_threads(run_context));
}

/*
vi:nowrap:ai:expandtab:sw=4:ts=4
*/
