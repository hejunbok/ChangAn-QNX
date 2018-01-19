#ifndef QTQNXCAR_UI_EXPORT_H
#define QTQNXCAR_UI_EXPORT_H

#include <qglobal.h>

#ifndef QTQNXCAR_UI_EXPORT
# if defined(QTQNXCAR_UI_STATIC_LIB)
   /* No export/import for static libraries */
#  define QTQNXCAR_UI_EXPORT
# elif defined(MAKE_QTQNXCAR_UI_LIB)
   /* We are building this library */
#  define QTQNXCAR_UI_EXPORT Q_DECL_EXPORT
# else
   /* We are using this library */
#  define QTQNXCAR_UI_EXPORT Q_DECL_IMPORT
# endif
#endif

# ifndef QTQNXCAR_UI_EXPORT_DEPRECATED
#  define QTQNXCAR_UI_EXPORT_DEPRECATED Q_DECL_DEPRECATED QTQNXCAR_UI_EXPORT
# endif

#endif
