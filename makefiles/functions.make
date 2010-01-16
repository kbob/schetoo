# -*- makefile-gmake -*-

comma := ,

# Evaluate to verify that a make variable is defined simply (nonrecursively).
define assert_simple
 ifeq "$$(flavor $1)" "recursive"
   $$(error $1 is defined recursively in $$(caller).  Use := )
 endif
endef

# Change all dir/libfoo.{so,a} => -Ldir -lfoo; pass others unchanged.
define munge_prereqs
 $(foreach l, $(filter-out %.a %.so, $1), $l) \
 $(foreach l, $(filter %.a, $1), \
              $(patsubst lib%.a, -L$(dir $l) -l%, $(notdir $l))) \
 $(foreach l, $(filter %.so, $1), \
              $(patsubst lib%.so,
                -Wl$(comma)-rpath$(comma)$(PWD)/$(dir $l) -L$(dir $l) -l%, \
                $(notdir $l)))
endef
