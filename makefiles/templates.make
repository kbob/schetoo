# -*- makefile-gmake -*-

# Template for a program.
define program_template

 # Verify that foo_cfiles, foo_yfiles, and foo_libs are nonrecursive.
 $$(eval $$(call assert_simple, $(1)_cfiles))
 $$(eval $$(call assert_simple, $(1)_yfiles))
 $$(eval $$(call assert_simple, $(1)_libs))

   $(2)_cfiles := $$($(1)_cfiles:%=$d%)
   $(2)_yfiles := $$($(1)_yfiles:%=$d%)
   $(2)_ofiles := $$($(2)_cfiles:.c=.o)
   $(2)_ofiles += $$($(2)_yfiles:.y=.o)
     $(2)_libs := $$($(1)_libs:=.$(libext))
   $(2)_ldlibs := $$($(1)_ldlibs)
        CFILES += $$($(2)_cfiles)
        YFILES += $$($(2)_yfiles)

 # foo's link rule
 $(2): $$($(2)_ofiles) $$($(2)_libs)
	$$(strip $$(LINK.o) $$(call munge_prereqs, $$^) \
                            $$($(2)_ldlibs) $$(LDLIBS) -o $$@)

endef


# Template for a library.
define lib_template

 # Verify that libfoo_cfiles is nonrecursive.
 $$(eval $$(call assert_simple, $(1)_cfiles))
 $$(eval $$(call assert_simple, $(1)_yfiles))

   $(2)_cfiles := $$($(1)_cfiles:%=$d%)
   $(2)_yfiles := $$($(1)_yfiles:%=$d%)
   $(2)_ofiles := $$($(2)_cfiles:%.c=%.o)
   $(2)_ofiles += $$($(2)_yfiles:%.y=%.o)
        CFILES += $$($(2)_cfiles)
        YFILES += $$($(2)_yfiles)

 vpath $(1).$$(libext) $$(dir $2)

 ifeq "$$(libtype)" "static"

  # libfoo's static link rule
  $(2).a: $$($(2)_ofiles)
	$$(AR) cr $$@ $$?

 else ifeq "$$(libtype)" "dynamic"

  export LD_LIBRARY_PATH = $$(dir $2):$(LD_LIBRARY_PATH)

  # libfoo's dynamic link rule
  $(2).so: $$($(2)_ofiles)
	$$(CC) -shared $$(LDFLAGS) $$(TARGET_ARCH) $$? -o $$@

 endif
endef


# Template for a test.
define test_template

 # Verify that testfoo_cfiles and testfoo_libs are nonrecursive.
 $$(eval $$(call assert_simple, $(1)_cfiles))
 $$(eval $$(call assert_simple, $(1)_yfiles))
 $$(eval $$(call assert_simple, $(1)_libs))
 $$(eval $$(call assert_simple, $(1)_CPPFLAGS))

   $(2)_cfiles := $$($(1)_cfiles:%=$d%)
   $(2)_yfiles := $$($(1)_yfiles:%=$d%)
   $(2)_ofiles := $$($(2)_cfiles:.c=.o)
   $(2)_ofiles += $$($(2)_yfiles:.y=.o)
     $(2)_libs := $$($(1)_libs:=.$(libext))
 $(2)_CPPFLAGS := $$($(1)_CPPFLAGS)
        CFILES += $$($(2)_cfiles)
        YFILES += $$($(2)_yfiles)

 $(2):	$$($(2)_ofiles) $$($(2)_libs)
	$$(strip $$(LINK.c) $$($(2)_CPPFLAGS) \
			    $$(call munge_prereqs, $$^) \
	                    $$($(2)_ldlibs) $$(LDLIBS) -o $$@)

endef


# Template for a subdirectory.
define dir_template

$2/Makefile: makefiles/module-makefile
	ln -s $3makefiles/module-makefile $$@

 # Reset the module variables, then include the subdirectory's makefile.
          dirs :=
      programs :=
          libs :=
  test_scripts :=
 test_programs :=
             d := $2/
          root := $3

 include $$(d)Dir.make
 include makefiles/directory.make
endef
