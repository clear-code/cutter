#include <gtk/gtk.h>

void gtk_test_warmup (void);

void
gtk_test_warmup (void)
{
    gtk_init(NULL, NULL);
}
