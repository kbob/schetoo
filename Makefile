# -*- makefile-gmake -*-

     FEATURES := -DSCHEME_FEATURE_UNICODE=1

     programs := scheme
 test_scripts := selftest.sh

  prim_groups := arith boolean char ctrl defn env eq eval expr list	\
		 proc string sym

    obj_types := binding bytevector boolean cont pair proc record rtd	\
		 string symbol vector

    mem_types := fixvec mixvec scalar

scheme_cfiles := main.c charbuf.c env.c eval.c except.c heap.c io.c	\
		 list.c low_ex.c oprintf.c print.c read.c roots.c	\
		 scan.c test.c unicode.c				\
									\
		 prim.c $(prim_groups:%=prim_%.c)			\
		 obj.c    $(obj_types:%=obj_%.c)			\
		 mem.c    $(mem_types:%=mem_%.c)

scheme_ldlibs := -lreadline


     CPPFLAGS := -std=c99 -D_XOPEN_SOURCE -iquote. $(FEATURES)
       CFLAGS := -g -Wall -Werror -Winline
      libtype := static

#    CPPFLAGS += -DNDEBUG -DNOTEST
#      CFLAGS := -O2

default: all

unicode.o .unicode.d: ucd_data.h

ucd_data.h: gen_ucd_data.py unicode.h UnicodeData.txt
	python gen_ucd_data.py > $@
JUNK += ucd_data.h

include makefiles/project-root.make
