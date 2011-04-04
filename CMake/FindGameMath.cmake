#
# Locate the libGameMath library.
# https://github.com/mgottschlag/GameMath
# Written by Mathias Gottschlag
# Based on FindTBB
# Written by Michael Hammer, michael _at_ derhammer.net

# This module defines
# GAMEMATH_INCLUDE_DIR, where to find ptlib.h, etc.
# GAMEMATH_LIBRARIES, the libraries to link against to use pwlib.
# GAMEMATH_FOUND, If false, don't try to use pwlib.

FIND_PATH(GAMEMATH_INCLUDE_DIR GameMath.hpp
    /usr/local/include
    /usr/include
)

# No libraries yet
SET(GAMEMATH_LIBRARIES)

SET(GAMEMATH_FOUND 0)
IF(GAMEMATH_INCLUDE_DIR)
  SET(TBB_FOUND 1)
  MESSAGE(STATUS "Found libGameMath")
ENDIF(GAMEMATH_INCLUDE_DIR)

MARK_AS_ADVANCED(
  GAMEMATH_INCLUDE_DIR
  GAMEMATH_LIBRARIES
)
