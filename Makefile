# -*- makefile-gmake -*-

#         dirs := lib
     programs := scheme
# test_scripts := selftest.sh

scheme_cfiles := except.c main.c roots.c \
	obj_bytevector.c obj_pair.c \
	mem.c mem_fixvec.c mem_scalar.c




#  scheme_libs := librnrs
#scheme_ldlibs := -Wl,--no-whole-archive -lreadline

#      CPPFLAGS := -D_GNU_SOURCE -iquote. 
       CFLAGS := -g -Wall -Werror -Winline
      libtype := static

default: all

unicode.o .unicode.d: ucd_data.h

ucd_data.h: gen_ucd_data.py unicode.h UnicodeData.txt
	python gen_ucd_data.py > $@
JUNK = ucd_data.h

include makefiles/project-root.make
