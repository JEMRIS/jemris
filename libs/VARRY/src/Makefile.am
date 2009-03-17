## Process this file with automake to produce Makefile.in

lib_LIBRARIES = libvar_array.a
include_HEADERS = var_array.h var_array++.h var_array++_cpp.h
libvar_array_a_SOURCES = var_array.c list.h list_c.h var_array.h

check_PROGRAMS = test1 test2 test3 test4 test5
TESTS = $(check_PROGRAMS)
LDADD = libvar_array.a

test3_SOURCES = test3.cpp
test4_SOURCES = test4.cpp
