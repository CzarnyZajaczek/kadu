# aclocal.m4 generated automatically by aclocal 1.6.3 -*- Autoconf -*-

# Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

dnl Based on AC_NEED_STDINT_H by Guido Draheim <guidod@gmx.de> that can be
dnl found at http://www.gnu.org/software/ac-archive/. Do not complain him
dnl about this macro.
dnl
dnl $Id: aclocal.m4,v 1.8 2003/01/12 23:12:59 adrian Exp $

AC_DEFUN([AC_NEED_STDINT_H],
 [AC_MSG_CHECKING([for uintXX_t types])

  if test "x$1" = "x"; then
    ac_stdint_h="stdint.h"
  else
    ac_stdint_h="$1"
  fi

  rm -f $ac_stdint_h

  ac_header_stdint=""
  for i in stdint.h inttypes.h sys/inttypes.h sys/int_types.h sys/types.h; do
    if test "x$ac_header_stdint" = "x"; then
      AC_TRY_COMPILE([#include <$i>], [uint32_t foo], [ac_header_stdint=$i])
    fi
  done

  if test "x$ac_header_stdint" != "x" ; then
    AC_MSG_RESULT([found in <$ac_header_stdint>])
    if test "x$ac_header_stdint" != "xstdint.h" ; then
      echo "#include <$ac_header_stdint>" > $ac_stdint_h
    fi
  else
    AC_MSG_RESULT([not found, using reasonable defaults])
    
    dnl let's make newer autoconf versions happy.
    stdint_h_foobar=define

    m4_pattern_allow([^__AC_STDINT_H$])
    
    cat > $ac_stdint_h << EOF
#ifndef __AC_STDINT_H
#$stdint_h_foobar __AC_STDINT_H 1

/* ISO C 9X: 7.18 Integer types <stdint.h> */

#define __int8_t_defined
typedef   signed char    int8_t;
typedef unsigned char   uint8_t;
typedef   signed short  int16_t;
typedef unsigned short uint16_t;
typedef   signed int    int32_t;
typedef unsigned int   uint32_t;

#endif /* __AC_STDINT_H */
EOF
  fi
])



