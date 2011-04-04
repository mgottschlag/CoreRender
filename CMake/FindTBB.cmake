#
# Locate Intel Threading Building Blocks include paths and libraries
# Written by Michael Hammer, michael _at_ derhammer.net

# This module defines
# TBB_INCLUDE_DIR, where to find ptlib.h, etc.
# TBB_LIBRARIES, the libraries to link against to use pwlib.
# TBB_FOUND, If false, don't try to use pwlib.

FIND_PATH(TBB_INCLUDE_DIR tbb/task_scheduler_init.h
    /usr/local/include
    /usr/include
)

FIND_LIBRARY(TBB_LIBRARIES
  NAMES
    tbb tbbmalloc
  PATHS
    /usr/local/lib
    /usr/lib
)

FIND_LIBRARY(TBB_LIBRARIES_DEBUG
  NAMES
    tbb_debug tbbmalloc_debug
  PATHS
    /usr/local/lib
    /usr/lib
)

SET(TBB_FOUND 0)
IF(TBB_INCLUDE_DIR)
  IF(TBB_LIBRARIES)
    SET(TBB_FOUND 1)
    MESSAGE(STATUS "Found Intel TBB")
  ENDIF(TBB_LIBRARIES)
ENDIF(TBB_INCLUDE_DIR)

MARK_AS_ADVANCED(
  TBB_INCLUDE_DIR
  TBB_LIBRARIES
)
