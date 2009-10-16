ADD_EXECUTABLE(pl_gui.wt
  ConfigOptions.cpp
  MRIBrowser.cpp
  PipelineApp.cpp
  ScanBrowser.cpp
  SubjectPage.cpp
)
TARGET_LINK_LIBRARIES(pl_gui.wt ${EXAMPLES_CONNECTOR})

INCLUDE_DIRECTORIES(
  ${WT_SOURCE_DIR}/src
)

#
# Create a deploy script
#
SET(APPNAME pl_gui)
SET(APPBIN pl_gui.wt)
SET(APPRESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/icons ${CMAKE_CURRENT_SOURCE_DIR}/*.xml ${CMAKE_CURRENT_SOURCE_DIR}/*.css")
#CONFIGURE_FILE(
#  ${WT_SOURCE_DIR}/deploy.sh
#  ${CMAKE_CURRENT_BINARY_DIR}/deploy.sh
#)

ADD_DEPENDENCIES(pl_gui.wt wt ${EXAMPLES_CONNECTOR})

