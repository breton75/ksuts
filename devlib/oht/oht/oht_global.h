#ifndef OHT_GLOBAL_H
#define OHT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(OHT_LIBRARY)
#  define OHTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define OHTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // OHT_GLOBAL_H
