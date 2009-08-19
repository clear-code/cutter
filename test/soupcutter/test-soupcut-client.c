#include <soupcutter/soupcutter.h>

void test_request(void);

static GError *error;

void
cut_setup (void)
{
    error = NULL;
}

void
cut_teardown (void)
{
    if (error)
        g_error_free(error);

}


void
test_request (void)
{
    SoupMessage *message;
    SoupCutClient *client;

    client = soupcut_client_new();
    message = soup_message_new("GET", "http://localhost/");
    soupcut_client_send_message(client, message);
    cut_assert_equal_uint(1, soupcut_client_get_n_messages(client));
    gcut_assert_equal_object(message, soupcut_client_get_latest_message(client));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
