#ifndef KANZI_EXPORT_H
#define KANZI_EXPORT_H

#include <qglobal.h>

#ifndef KANZI_EXPORT
# if defined(KANZI_STATIC_LIB)
   /* No export/import for static libraries */
#  define KANZI_EXPORT
# elif defined(MAKE_KANZI_LIB)
   /* We are building this library */
#  define KANZI_EXPORT Q_DECL_EXPORT
# else
   /* We are using this library */
#  define KANZI_EXPORT Q_DECL_IMPORT
# endif
#endif

# ifndef KANZI_EXPORT_DEPRECATED
#  define KANZI_EXPORT_DEPRECATED Q_DECL_DEPRECATED KANZI_EXPORT
# endif

#endif
