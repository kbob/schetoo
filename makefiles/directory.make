# -*-makefile-gmake-*-

# caller is the current directory's makefile.
# d is the directory where caller lives.
        caller := $(lastword $(filter-out makefiles/%, $(MAKEFILE_LIST)))
             d := $(dir $(caller))
# $(info From $(caller))

# Verify that module variables are nonrecursive.
   module_vars := dirs programs libs tests
$(foreach v, $(module_vars), $(eval $(call assert_simple, $v)))

# Collect the subdirectories and targets.
          DIRS += $(dirs:%=$d%)
      PROGRAMS += $(programs:%=$d%)
          LIBS += $(libs:%=$d%.$(libext))
  TEST_SCRIPTS += $(test_scripts:%=$d%)
 TEST_PROGRAMS += $(test_programs:%=$d%)

# Expand the templates for all the programs, libs, tests, and dirs in
# this module.
$(foreach p, $(programs),      $(eval $(call program_template,$p,$d$p)))
$(foreach l, $(libs),          $(eval $(call     lib_template,$l,$d$l)))
$(foreach t, $(test_programs), $(eval $(call    test_template,$t,$d$t)))
$(foreach i, $(dirs),          $(eval $(call dir_template,$i,$d$i,../$(root))))
