#include <soupcutter/soupcutter.h>

#define SOUPCUT_TEST_PORT 33333

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
    SoupAddress *address;

    client = soupcut_client_new();

    address = soup_address_new("localhost", SOUPCUT_TEST_PORT);
    soup_address_resolve_sync(address, NULL);
    server = soup_server_new(SOUP_SERVER_INTERFACE, address,
                             SOUP_SERVER_ASYNC_CONTEXT,
                             soupcut_client_get_async_context(client),
                             NULL);
    g_object_unref(address);

    serve(server);

    message = soup_message_new("GET",
                               cut_take_printf("http://localhost:%u/",
                                               SOUPCUT_TEST_PORT));
    cut_assert_equal_uint(SOUP_STATUS_OK,
                          soupcut_client_send_message(client, message));
    
    soup_server_quit(server);
    g_object_unref(server);

    cut_assert_equal_string("text/plain", soup_message_headers_get_content_type(message->response_headers, NULL));
    cut_assert_equal_uint(1, soupcut_client_get_n_messages(client));
    gcut_assert_equal_object(message, soupcut_client_get_latest_message(client));
}

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
