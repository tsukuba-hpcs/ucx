uct_modules="${uct_modules}:utofu"
AC_CONFIG_FILES([src/uct/utofu/Makefile])

AC_CHECK_FILE([/lib64/libtofucom.so], 
              [AC_DEFINE([HAVE_UTOFU], [1], [Define to 1 if libtofucom.so is available.])])

AM_CONDITIONAL([HAVE_UTOFU], [test "x$ac_cv_file__lib64_libtofucom_so" = "xyes"])