# -*- makefile-gmake -*-

#         dirs := lib
     programs := scheme
 test_scripts := selftest.sh

scheme_cfiles := main.c charbuf.c except.c io.c print.c read.c roots.c scan.c \
	test.c unicode.c \
	obj_bytevector.c obj_boolean.c obj_pair.c obj_string.c obj_symbol.c \
	obj_vector.c \
	mem.c mem_fixvec.c mem_scalar.c




#  scheme_libs := librnrs
#scheme_ldlibs := -Wl,--no-whole-archive -lreadline
scheme_ldlibs := -lreadline

#      CPPFLAGS := -D_GNU_SOURCE -iquote. 
       CFLAGS := -g -Wall -Werror -Winline -std=c99 -DSCHEME_FEATURE_UNICODE=1 \
		 -iquote.
      libtype := static

default: all

unicode.o .unicode.d: ucd_data.h

ucd_data.h: gen_ucd_data.py unicode.h UnicodeData.txt
	python gen_ucd_data.py > $@
JUNK = ucd_data.h

include makefiles/project-root.make
