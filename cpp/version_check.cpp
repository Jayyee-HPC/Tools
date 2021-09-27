
/*!
 The version of C++ standard that is being used.
 The C++17 standard.
*/
#ifndef MY_HAS_CPP17
# ifdef _MSVC_LANG
#  if _MSVC_LANG > 201402L
#   define MY_HAS_CPP17 1
#  else /* _MSVC_LANG > 201402L */
#   define MY_HAS_CPP17 0
#  endif /* _MSVC_LANG > 201402L */
# else /* _MSVC_LANG */
#  if __cplusplus > 201402L
#   define MY_HAS_CPP17 1
#  else /* __cplusplus > 201402L */
#   define MY_HAS_CPP17 0
#  endif /* __cplusplus > 201402L */
 #endif /* _MSVC_LANG */
#endif /* MY_HAS_CPP17 */
 
/*!
 The version of C++ standard that is being used.
 The C++14 standard.
*/
#ifndef MY_HAS_CPP14
# ifdef _MSVC_LANG
#  if _MSVC_LANG > 201103L
#   define MY_HAS_CPP14 1
#  else /* _MSVC_LANG > 201103L */
#   define MY_HAS_CPP14 0
#  endif /* _MSVC_LANG > 201103L */
# else /* _MSVC_LANG */
#  if __cplusplus > 201103L
#   define MY_HAS_CPP14 1
#  else /* __cplusplus > 201103L */
#   define MY_HAS_CPP14 0
#  endif /* __cplusplus > 201103L */
# endif /* _MSVC_LANG */
#endif /* MY_HAS_CPP14 */
 
/*!
 The version of C++ standard that is being used.
 The C++11 standard.
*/
#ifndef MY_HAS_CPP11
# ifdef _MSVC_LANG
#  if _MSVC_LANG > 199711L
#   define MY_HAS_CPP11 1
#  else /* _MSVC_LANG > 199711L */
#   define MY_HAS_CPP11 0
#  endif /* _MSVC_LANG > 199711L */
# else /* _MSVC_LANG */
#  if __cplusplus > 199711L
#   define MY_HAS_CPP11 1
#  else /* __cplusplus > 199711L */
#   define MY_HAS_CPP11 0
#  endif /* __cplusplus > 199711L */
# endif /* _MSVC_LANG */
#endif /* MY_HAS_CPP11 */