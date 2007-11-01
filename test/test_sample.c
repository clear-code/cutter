
void
test_function1 (void)
{

}

void
test_function2 (void)
{

}

static CutTestStruct cut_tests[] =
{
    {"test1", test_function1},
    {"test2", test_function2}
};
static const gint cut_tests_len = G_N_ELEMENTS(tests);

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
