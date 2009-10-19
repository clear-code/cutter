SUBDIRS = 			\
	lib			\
	fixtures		\
	cutter			\
	gcutter			\
	cppcutter		\
	gdkcutter-pixbuf	\
	soupcutter

EXTRA_DIST = run-test.sh

TESTS = run-test.sh
TESTS_ENVIRONMENT = 		\
	NO_MAKE=yes 		\
	CUTTER="$(CUTTER)"

echo-cutter:
	@echo $(CUTTER)
