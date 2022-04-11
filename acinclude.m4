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

