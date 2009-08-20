#include <soupcutter/soupcutter.h>
#include "../lib/cuttest-soup.h"

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

static void
server_callback(SoupServer *server,
                SoupMessage *msg,
                const gchar *path,
                GHashTable *query,
                SoupClientContext *client,
                gpointer user_data)
{
    soup_message_set_status(msg, SOUP_STATUS_OK);
    soup_message_set_response(msg, "text/plain", SOUP_MEMORY_COPY,
                              "Hello", 5);
}

static gpointer
serve(gpointer data)
{
    SoupServer *server = data;

    soup_server_add_handler(server, "/", server_callback,
                            NULL, NULL);
    soup_server_run_async(server);

    return NULL;
}

void
test_request (void)
{
    SoupMessage *message;
    SoupCutClient *client;
    SoupServer *server;
    GMainContext *context;
    const gchar *uri;

    client = soupcut_client_new();
    context = soupcut_client_get_async_context(client);
    server = cuttest_soup_server_take_new(context);
    uri = cuttest_soup_server_build_uri(server, "/");
    serve(server);

    message = soup_message_new("GET", uri);

    cut_assert_equal_uint(SOUP_STATUS_OK,
                          soupcut_client_send_message(client, message));

     cut_assert_equal_string("text/plain", soup_message_headers_get_content_type(message->response_headers, NULL));
    cut_assert_equal_uint(1, soupcut_client_get_n_messages(client));
    gcut_assert_equal_object(message, soupcut_client_get_latest_message(client));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
