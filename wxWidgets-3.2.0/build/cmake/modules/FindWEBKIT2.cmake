# - Find Webkit2
# Find the Webkit2 includes and library
#
#  WEBKIT2_INCLUDE_DIR - Where to find Webkit2 include sub-directory.
#  WEBKIT2_LIBRARIES   - List of libraries when using Webkit2.
#  WEBKIT2_FOUND       - True if Webkit2 found.

SET( WEBKIT2_VERSION 4.0)

set(WEBKIT2_INCLUDE_DIR WEBKIT2_INCLUDE_DIR-NOTFOUND)
set(WEBKIT2_LIBRARY WEBKIT2_LIBRARY-NOTFOUND)
set(WEBKIT2_JS_LIBRARY WEBKIT2_JS_LIBRARY-NOTFOUND)
set(WEBKIT2_LIBRARIES WEBKIT2_LIBRARIES-NOTFOUND)

FIND_PATH(WEBKIT2_INCLUDE_DIR webkit2/webkit2.h
    PATH_SUFFIXES "webkitgtk-${WEBKIT2_VERSION}"
)

SET(WEBKIT2_NAMES "webkit2gtk-${WEBKIT2_VERSION}")
FIND_LIBRARY(WEBKIT2_LIBRARY
    NAMES ${WEBKIT2_NAMES}
)

SET(WEBKIT2_JS_NAMES "javascriptcoregtk-${WEBKIT2_VERSION}")
FIND_LIBRARY(WEBKIT2_JS_LIBRARY
    NAMES ${WEBKIT2_JS_NAMES}
)

# Handle the QUIETLY and REQUIRED arguments and set WEBKIT2_FOUND to
# TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    WEBKIT2 DEFAULT_MSG
    WEBKIT2_LIBRARY WEBKIT2_INCLUDE_DIR
)

IF(WEBKIT2_FOUND)
    SET( WEBKIT2_LIBRARIES ${WEBKIT2_LIBRARY} ${WEBKIT2_JS_LIBRARY} )
ELSE(WEBKIT2_FOUND)
    SET( WEBKIT2_LIBRARIES )
ENDIF(WEBKIT2_FOUND)

MARK_AS_ADVANCED(WEBKIT2_LIBRARY
                 WEBKIT2_LIBRARIES
                 WEBKIT2_INCLUDE_DIR
                 WEBKIT2_JS_LIBRARY
)
