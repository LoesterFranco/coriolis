# - Try to find the IO libraries
# Once done this will define
#
#  IO_FOUND - system has the IO library
#  IO_INCLUDE_DIR - the IO include directory
#  IO_LIBRARIES - The libraries needed to use IO

SET(IO_DIR_SEARCH $ENV{IO_TOP})

SET(IO_FOUND FALSE)
# AGDS
FIND_PATH(AGDS_INCLUDE_DIR
          NAMES io/agds/GdsLibrary.h 
          PATHS ${IO_DIR_SEARCH}
          PATH_SUFFIXES include
)
FIND_LIBRARY(AGDS_LIBRARY
             NAMES agds
             PATHS ${IO_DIR_SEARCH}
             PATH_SUFFIXES lib
)
IF(AGDS_INCLUDE_DIR AND AGDS_LIBRARY)
   SET(AGDS_FOUND TRUE)
  #SET(IO_FOUND   TRUE)
#   SET(IO_LIBRARIES ${AGDS_LIBRARY})
ELSE(AGDS_INCLUDE_DIR AND AGDS_LIBRARY)
   SET(AGDS_FOUND FALSE)
#   SET(IO_LIBRARIES)
ENDIF(AGDS_INCLUDE_DIR AND AGDS_LIBRARY)

# OPENCHAMS
FIND_PATH(OPENCHAMS_INCLUDE_DIR
          NAMES io/openChams/Circuit.h 
          PATHS ${IO_DIR_SEARCH}
          PATH_SUFFIXES include
)
FIND_LIBRARY(OPENCHAMS_LIBRARY
             NAMES openChams
             PATHS ${IO_DIR_SEARCH}
             PATH_SUFFIXES lib
)
IF(OPENCHAMS_INCLUDE_DIR AND OPENCHAMS_LIBRARY)
   SET(OPENCHAMS_FOUND TRUE)
  #SET(IO_FOUND   TRUE)
#   SET(IO_LIBRARIES ${OPENCHAMS_LIBRARY})
ELSE(OPENCHAMS_INCLUDE_DIR AND OPENCHAMS_LIBRARY)
   SET(OPENCHAMS_FOUND FALSE)
#   SET(IO_LIBRARIES)
ENDIF(OPENCHAMS_INCLUDE_DIR AND OPENCHAMS_LIBRARY)

IF(AGDS_FOUND AND OPENCHAMS_FOUND)
    SET(IO_FOUND TRUE)
ELSE(AGDS_FOUND AND OPENCHAMS_FOUND)
    SET(IO_FOUND FALSE)
ENDIF(AGDS_FOUND AND OPENCHAMS_FOUND)

IF (NOT IO_FOUND)
    SET(IO_MESSAGE
    "IO libraries were not found. Make sure IO_TOP env variable is set.")
    IF (NOT IO_FIND_QUIETLY)
        MESSAGE(STATUS "${IO_MESSAGE}")
    ELSE(NOT IO_FIND_QUIETLY)
        IF(IO_FIND_REQUIRED)
            MESSAGE(FATAL_ERROR "${IO_MESSAGE}")
        ENDIF(IO_FIND_REQUIRED)
    ENDIF(NOT IO_FIND_QUIETLY)
ELSE (NOT IO_FOUND)
    MESSAGE(STATUS "IO library was found in ${IO_DIR_SEARCH}")
ENDIF (NOT IO_FOUND)
