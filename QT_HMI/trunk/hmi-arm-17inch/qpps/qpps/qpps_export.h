#ifndef QPPS_EXPORT_H
#define QPPS_EXPORT_H

#include <qglobal.h>

#ifndef QPPS_EXPORT
# if defined(QPPS_STATIC_LIB)
   /* No export/import for static libraries */
#  define QPPS_EXPORT
# elif defined(MAKE_QPPS_LIB)
   /* We are building this library */
#  define QPPS_EXPORT Q_DECL_EXPORT
# else
   /* We are using this library */
#  define QPPS_EXPORT Q_DECL_IMPORT
# endif
#endif

# ifndef QPPS_EXPORT_DEPRECATED
#  define QPPS_EXPORT_DEPRECATED Q_DECL_DEPRECATED QPPS_EXPORT
# endif

#endif
