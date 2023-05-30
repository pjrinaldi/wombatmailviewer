
#ifndef VMIME_EXPORT_H
#define VMIME_EXPORT_H

#ifdef VMIME_STATIC
#  define VMIME_EXPORT
#  define VMIME_NO_EXPORT
#else
#  ifndef VMIME_EXPORT
#    ifdef vmime_EXPORTS
        /* We are building this library */
#      define VMIME_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define VMIME_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef VMIME_NO_EXPORT
#    define VMIME_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef VMIME_DEPRECATED
#  define VMIME_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef VMIME_DEPRECATED_EXPORT
#  define VMIME_DEPRECATED_EXPORT VMIME_EXPORT VMIME_DEPRECATED
#endif

#ifndef VMIME_DEPRECATED_NO_EXPORT
#  define VMIME_DEPRECATED_NO_EXPORT VMIME_NO_EXPORT VMIME_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef VMIME_NO_DEPRECATED
#    define VMIME_NO_DEPRECATED
#  endif
#endif

#endif /* VMIME_EXPORT_H */
