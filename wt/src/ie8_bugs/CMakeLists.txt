
ADD_EXECUTABLE(ie8_bugs.wt
  MRIBrowser.cpp
  MRIInfoBox.cpp
  PatientInfoBox.cpp
  PipelineApp.cpp
  ScanBrowser.cpp
  SelectScans.cpp
  SubjectPage.cpp
)

TARGET_LINK_LIBRARIES(ie8_bugs.wt wt ${EXAMPLES_CONNECTOR} ${BOOST_WT_LIBRARIES} ${BOOST_FS_LIB_MT})

INCLUDE_DIRECTORIES(
  ${WT_SOURCE_DIR}/src
)

#
# Create a deploy script
#
SET(APPNAME ie8_bugs)
SET(APPBIN ie8_bugs.wt)
SET(APPRESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/icons ${CMAKE_CURRENT_SOURCE_DIR}/*.xml ${CMAKE_CURRENT_SOURCE_DIR}/*.css ${CMAKE_CURRENT_SOURCE_DIR}/conf/")
CONFIGURE_FILE(
  ${WT_SOURCE_DIR}/deploy.sh
  ${CMAKE_CURRENT_BINARY_DIR}/deploy.sh
)

ADD_DEPENDENCIES(ie8_bugs.wt wt ${EXAMPLES_CONNECTOR})