#ifndef QTQNXCAR2_EXPORT_H
#define QTQNXCAR2_EXPORT_H

#include <qglobal.h>

#ifndef QTQNXCAR2_EXPORT
# if defined(QTQNXCAR2_STATIC_LIB)
   /* No export/import for static libraries */
#  define QTQNXCAR2_EXPORT
# elif defined(MAKE_QTQNXCAR2_LIB)
   /* We are building this library */
#  define QTQNXCAR2_EXPORT Q_DECL_EXPORT
# else
   /* We are using this library */
#  define QTQNXCAR2_EXPORT Q_DECL_IMPORT
# endif
#endif

# ifndef QTQNXCAR2_EXPORT_DEPRECATED
#  define QTQNXCAR2_EXPORT_DEPRECATED Q_DECL_DEPRECATED QTQNXCAR2_EXPORT
# endif

#endif
