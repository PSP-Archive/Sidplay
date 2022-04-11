dnl aclocal.m4 generated automatically by aclocal 1.4-p5

dnl Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

dnl -------------------------------------------------------------------------
dnl Try to find a file (or one of more files in a list of dirs).
dnl -------------------------------------------------------------------------

AC_DEFUN(MY_FIND_FILE,
    [
    $3=NO
    for i in $2;
    do
        for j in $1;
        do
	        if test -r "$i/$j"; then
		        $3=$i
                break 2
            fi
        done
    done
    ]
)

dnl -------------------------------------------------------------------------
dnl Try to find SIDPLAY includes and library.
dnl $my_have_sidplay will be "yes" or "no"
dnl $(sidplay_libdir) will be path to libsidplay.*
dnl $(sidplay_incdir) will be path to sidplay/*.h
dnl @SIDPLAY_LDFLAGS@ will be substituted with -L$sidplay_libdir
dnl @SIDPLAY_INCLUDES@ will be substituted with -I$sidplay_incdir
dnl -------------------------------------------------------------------------

AC_DEFUN(MY_PATH_LIBSIDPLAY,
[
    AC_MSG_CHECKING([for working SIDPLAY library and headers])
    
    dnl Be pessimistic.
    my_sidplay_library=NO
    my_sidplay_includes=NO
    sidplay_libdir=""
    sidplay_incdir=""

    AC_ARG_WITH(sidplay-includes,
        [  --with-sidplay-includes=DIR
                          where the sidplay includes are located],
        [my_sidplay_includes="$withval"]
    )

    AC_ARG_WITH(sidplay-library,
        [  --with-sidplay-library=DIR
                          where the sidplay library is installed],
        [my_sidplay_library="$withval"]
    )

    # Test compilation with library and headers in standard path.
    my_sidplay_libadd=""
    my_sidplay_incadd=""

    # Use library path given by user (if any).
    if test "$my_sidplay_library" != NO; then
        my_sidplay_libadd="-L$my_sidplay_library"
    fi

    # Use include path given by user (if any).
    if test "$my_sidplay_includes" != NO; then
        my_sidplay_incadd="-I$my_sidplay_includes"
    fi

    # Run test compilation.
    AC_CACHE_VAL(my_cv_libsidplay_works,
    [
        MY_TRY_LIBSIDPLAY
        my_cv_libsidplay_works=$my_libsidplay_works
    ])

    if test "$my_cv_libsidplay_works" = no; then
    
        # Test compilation failed.
        # Need to search for library and headers.
        AC_CACHE_VAL(my_cv_have_sidplay,
        [
            # Search common locations where header files might be stored.
            sidplay_incdirs="$my_sidplay_includes /usr/include /usr/local/include /usr/lib/sidplay/include /usr/local/lib/sidplay/include"
            MY_FIND_FILE(sidplay/sidtune.h,$sidplay_incdirs,sidplay_foundincdir)
            my_sidplay_includes=$sidplay_foundincdir

            # Search common locations where library might be stored.
            sidplay_libdirs="$my_sidplay_library /usr/lib /usr/local/lib /usr/lib/sidplay /usr/local/lib/sidplay"
            MY_FIND_FILE(libsidplay.so libsidplay.so.1 libsidplay.so.1.36 libsidplay.so.1.37,$sidplay_libdirs,sidplay_foundlibdir)
            my_sidplay_library=$sidplay_foundlibdir

            if test "$my_sidplay_includes" = NO || test "$my_sidplay_library" = NO; then
                my_cv_have_sidplay="my_have_sidplay=no"
            else
                my_cv_have_sidplay="my_have_sidplay=yes"
            fi
        ])
        
        eval "$my_cv_have_sidplay"
        if test "$my_have_sidplay" = yes; then
            sidplay_libadd="-L$my_sidplay_library"
            sidplay_incadd="-I$my_sidplay_includes"
            
            # Test compilation with found paths.
            AC_CACHE_VAL(my_cv_found_libsidplay_works,
            [
                MY_TRY_LIBSIDPLAY
                my_cv_found_libsidplay_works=$my_libsidplay_works
            ])
            my_have_sidplay=$my_cv_found_libsidplay_works
            
            if test "$my_have_sidplay" = no; then
                # Found library does not link without errors.
                my_have_sidplay=no
                AC_MSG_RESULT([$my_have_sidplay]);
            else
                AC_MSG_RESULT([library $my_sidplay_library, headers $my_sidplay_includes])
            fi
        else
            # Either library or headers not found.
            AC_MSG_RESULT([$my_have_sidplay]);
        fi
    else
        # Simply print 'yes' without printing the standard path.
        my_have_sidplay=yes
        AC_MSG_RESULT([$my_have_sidplay]);
    fi

    AC_SUBST(sidplay_libdir)
    AC_SUBST(sidplay_incdir)

    SIDPLAY_LDFLAGS="$my_sidplay_libadd"
    SIDPLAY_INCLUDES="$my_sidplay_incadd"

    AC_SUBST(SIDPLAY_LDFLAGS)
    AC_SUBST(SIDPLAY_INCLUDES)
])

dnl Function used by MY_PATH_LIBSIDPLAY.

AC_DEFUN(MY_TRY_LIBSIDPLAY,
[
    my_cxxflags_save=$CXXFLAGS
    my_ldflags_save=$LDFLAGS
    my_libs_save=$LIBS

    CXXFLAGS="$CXXFLAGS $my_sidplay_incadd"
    LDFLAGS="$LDFLAGS $my_sidplay_libadd"
    LIBS="-lsidplay"

    AC_TRY_LINK(
        [#include <sidplay/sidtune.h>],
        [sidTune* myTest;],
        [my_libsidplay_works=yes],
        [my_libsidplay_works=no]
    )

    CXXFLAGS="$my_cxxflags_save"
    LDFLAGS="$my_ldflags_save"
    LIBS="$my_libs_save"
])


# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN([AM_INIT_AUTOMAKE],
[AC_REQUIRE([AC_PROG_INSTALL])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of package])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version number of package]))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN([AM_SANITY_CHECK],
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN([AM_MISSING_PROG],
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])

