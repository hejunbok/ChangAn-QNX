#ifndef QPLAYER_EXPORT_H
#define QPLAYER_EXPORT_H

#include <qglobal.h>

#ifndef QPLAYER_EXPORT
# if defined(QPLAYER_STATIC_LIB)
   /* No export/import for static libraries */
#  define QPLAYER_EXPORT
# elif defined(MAKE_QPLAYER_LIB)
   /* We are building this library */
#  define QPLAYER_EXPORT Q_DECL_EXPORT
# else
   /* We are using this library */
#  define QPLAYER_EXPORT Q_DECL_IMPORT
# endif
#endif

# ifndef QPLAYER_EXPORT_DEPRECATED
#  define QPLAYER_EXPORT_DEPRECATED Q_DECL_DEPRECATED QPLAYER_EXPORT
# endif

#endif
