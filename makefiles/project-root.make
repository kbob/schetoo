# -*- makefile-gmake -*-

     MAKEFLAGS += -j

# Will we build static or dynamic libraries?

 libtype ?= dynamic
ifeq "$(libtype)" "static"
        libext := a
else ifeq "$(libtype)" "dynamic"
        libext := so
else
 $(error unknown libtype "$(libtype)")
endif

          DIRS :=
      PROGRAMS :=
          LIBS :=
  TEST_SCRIPTS :=
 TEST_PROGRAMS :=

          root :=

include makefiles/functions.make
include makefiles/templates.make
include makefiles/directory.make

TESTS := $(TEST_SCRIPTS) $(TEST_PROGRAMS)

.PHONY: default help all test tests build programs libs clean

help:
	@echo 'Common Targets'
	@echo '    all (default) - build everything, run all tests'
	@echo '    test          - run all tests'
	@echo '    build         - build everything'
	@echo '    programs      - build all programs'
	@echo '    libs          - build all libraries'
	@echo '    tests         - build all tests'
	@echo '    Makefiles     - make Makefiles in all subdirectories'
	@echo '    clean         - remove generated files'
	@echo ''
	@echo 'Individual Programs'
	@$(foreach p, $(PROGRAMS),      echo '    $(patsubst ./%,%,$p)';)
	@echo ''
	@echo 'Individual Libraries'
	@$(foreach l, $(LIBS),          echo '    $(patsubst ./%,%,$l)';)
	@echo ''
	@echo 'Individual Test Programs'
	@$(foreach t, $(TEST_PROGRAMS), echo '    $(patsubst ./%,%,$t)';)
	@echo ''
	@echo 'Individual Test Scripts'
	@$(foreach t, $(TEST_SCRIPTS), echo '    $(patsubst ./%,%,$t)';)
	@echo ''

all:	build test

test:	tests
	@$(foreach t, $(TESTS), \
	    echo 'Test $(patsubst ./%,%,$t)'; \
	    $t;)

build:	Makefiles libs programs tests

programs: $(PROGRAMS)

libs:	$(LIBS)

tests:	$(TESTS)

Makefiles: $(patsubst ./%, %, $(DIRS:%=%/Makefile))

junk := *~ *.o *.so *.a .*.d a.out core TAGS $(JUNK)
clean:
	rm -f $(patsubst ./%,%,$(PROGRAMS))
	rm -f $(patsubst ./%,%,$(LIBS))
	rm -f $(patsubst ./%,%,$(TEST_PROGRAMS))
	rm -f $(patsubst ./%,%,$(YFILES:.y=.c))
	@echo 'rm -f    $(junk)'; rm -f $(junk)
	@echo '# junk = $(junk) Makefile'
	@$(foreach d, $(DIRS), \
            echo 'rm -f [junk in $(subst ./,,$d)]'; \
            rm -f $(subst ./,,$(foreach x, $(junk), $d/$x) $d/Makefile);)

$(TEST_SCRIPTS): $(PROGRAMS)

# C source dependency generation.
.%.d %/.%.d: %.c
	@rm -f "$@"
	@$(CC) -M -MP -MT '$*.o $@' -MF $@ $(CPPFLAGS) $< || rm -f "$@"

.%.d %/.%.d: %.y
	@rm -f $@
	@$(YACC.y) -o $*.c $<
	@$(CC) -M -MP -MT '$*.o $@' $(CPPFLAGS) $*.c | sed 's/$*.c/$</' > $@ \
		|| rm -f $@
	@rm -f $*.c

%.o: %.y
	$(YACC.y) -o $*.c $<
	$(COMPILE.c) $(OUTPUT_OPTION) $*.c || { rm -f $*.c; false; }
	rm $*.c

.DELETE_ON_ERROR:

-include $(join $(dir $(CFILES)), $(patsubst %.c, .%.d, $(notdir $(CFILES))))
-include $(join $(dir $(YFILES)), $(patsubst %.y, .%.d, $(notdir $(YFILES))))
