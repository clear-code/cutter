#include "cuttest-utils.h"

const gchar *
cuttest_get_base_dir(void)
{
    const gchar *dir;

    dir = g_getenv("BASE_DIR");
    return dir ? dir : ".";
}
