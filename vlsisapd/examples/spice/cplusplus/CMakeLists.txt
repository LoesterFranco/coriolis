INCLUDE_DIRECTORIES ( ${VLSISAPD_SOURCE_DIR}/src/spice/src )
ADD_EXECUTABLE ( driveSpice driveSpice.cpp )
ADD_EXECUTABLE ( parseSpice parseSpice.cpp )
TARGET_LINK_LIBRARIES ( driveSpice spice ) # 'driveSpice' is the name of the executable and 'spice' the name of the target library in openChams/src/CMakeLists.txt
TARGET_LINK_LIBRARIES ( parseSpice spice )
INSTALL ( TARGETS driveSpice     parseSpice     DESTINATION share/doc/coriolis2/examples/vlsisapd/spice )
INSTALL ( FILES   driveSpice.cpp parseSpice.cpp DESTINATION share/doc/coriolis2/examples/vlsisapd/spice )
INSTALL ( FILES   cmake.ex                      DESTINATION share/doc/coriolis2/examples/vlsisapd/spice RENAME CMakeLists.txt )
