ADD_EXECUTABLE(pl_gui.wt
  ConfigOptions.cpp
  ConfigXML.cpp
  ClusterJobBrowser.cpp
  ClusterLoadChart.cpp
  FileBrowser.cpp
  FilePreviewBox.cpp
  LogFileBrowser.cpp
  LogFileTailer.cpp
  MonitorLogTab.cpp
  MonitorPage.cpp
  MonitorResultsTab.cpp 
  MRIBrowser.cpp
  MRIInfoBox.cpp
  PatientInfoBox.cpp
  PipelineApp.cpp
  PipelineConfigure.cpp
  PipelineOptions.cpp
  PipelineOptionsFS.cpp
  PipelineOptionsTract.cpp
  PipelineStatus.cpp
  ResultsBrowser.cpp
  ScanBrowser.cpp
  SelectScans.cpp
  SubjectPage.cpp
)
TARGET_LINK_LIBRARIES(pl_gui.wt wt ${EXAMPLES_CONNECTOR} ${BOOST_WT_LIBRARIES} ${BOOST_FS_LIB_MT} mxml)

INCLUDE_DIRECTORIES(
  ${WT_SOURCE_DIR}/src
)

#
# Create a deploy script
#
SET(APPNAME pl_gui)
SET(APPBIN pl_gui.wt)
SET(APPRESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/icons ${CMAKE_CURRENT_SOURCE_DIR}/*.css ${CMAKE_CURRENT_SOURCE_DIR}/conf/")
CONFIGURE_FILE(
  ${WT_SOURCE_DIR}/deploy.sh
  ${CMAKE_CURRENT_BINARY_DIR}/deploy.sh
)

ADD_DEPENDENCIES(pl_gui.wt wt ${EXAMPLES_CONNECTOR})