# -*- makefile-gmake -*-

     FEATURES := -DSCHEME_FEATURE_UNICODE=1

     programs := scheme
 test_scripts := selftest.sh

scheme_cfiles := main.c charbuf.c env.c eval.c except.c heap.c io.c	\
		 low_ex.c oprintf.c print.c proc.c read.c roots.c	\
		 scan.c test.c unicode.c				\
									\
		 prim_arith.c						\
									\
		 obj_binding.c obj_bytevector.c obj_boolean.c		\
		 obj_cont.c obj_pair.c obj_procedure.c obj_string.c	\
		 obj_symbol.c obj_vector.c				\
									\
		 mem.c mem_fixvec.c mem_mixvec.c mem_scalar.c

#  scheme_libs := librnrs
#scheme_ldlibs := -Wl,--no-whole-archive -lreadline
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
JUNK = ucd_data.h

include makefiles/project-root.make
