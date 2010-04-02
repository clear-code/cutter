tests =  \
	stub-namespace.dll

stub-namespace.dll: stub-namespace.obj $(top_srcdir)\vcc\resource\test\fixtures\loader\cpp\stub-namespace.def
	$(CC) $(CFLAGS) $(stub_namespace_CFLAGS) -LD -Fe$@ stub-namespace.obj $(LIBS) $(LDFLAGS) $(stub_namespace_LDFLAGS) /def:$(top_srcdir)\vcc\resource\test\fixtures\loader\cpp\stub-namespace.def

