SUBDIRS(lib)

QT4_GENERATE_MOC(${CMAKE_CURRENT_SOURCE_DIR}/QtFileSystemWatcher.h
                 ${CMAKE_CURRENT_BINARY_DIR}/moccedQtFileSystemWatcher.cpp)

ADD_EXECUTABLE(pl_gui.wt
  ArchiveFileResource.cpp
  ConfigOptions.cpp
  ConfigXML.cpp
  ClusterJobBrowser.cpp
  ClusterLoadChart.cpp
  ClusterLoadPage.cpp
  FileBrowser.cpp
  FilePreviewBox.cpp
  JobStatus.cpp
  LogFileBrowser.cpp
  LogFileTailer.cpp
  LoginPage.cpp
  MonitorLogTab.cpp
  MonitorResultsTab.cpp
  MRIBrowser.cpp
  MRIInfoBox.cpp
  PatientInfoBox.cpp
  PermissionsXML.cpp
  PipelineApp.cpp
  PipelineArgTable.cpp
  PipelineConfigure.cpp
  PipelineOptions.cpp
  PipelineOptionsConnectome.cpp
  PipelineOptionsDcmSend.cpp
  PipelineOptionsFetal.cpp
  PipelineOptionsFS.cpp
  PipelineOptionsTract.cpp
  PipelineStatus.cpp
  ProjectChooser.cpp
  ProjectCreator.cpp
  ProjectPage.cpp
  ProjectXML.cpp
  ResultsBrowser.cpp
  ResultsFilterProxyModel.cpp
  ResultsPage.cpp
  ResultsTable.cpp
  ScanBrowser.cpp
  ScansToProcessTable.cpp
  SearchTerm.cpp
  SelectScans.cpp
  SubjectPage.cpp
  SubmitJobDialog.cpp
  QtFileSystemWatcher.cpp
  QtFileSystemWatcherThread.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/moccedQtFileSystemWatcher.cpp
)

TARGET_LINK_LIBRARIES(pl_gui.wt wt ${EXAMPLES_CONNECTOR} ${QT_LIBRARIES} wtwithqt ${BOOST_WT_LIBRARIES} ${BOOST_FS_LIB_MT} ${SSL_LIBRARIES} mxml)

INCLUDE_DIRECTORIES(
  ${WT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
  ${QT_QTCORE_INCLUDE_DIR} ${QT_INCLUDE_DIR}
)

#
# Create a deploy script
#
SET(APPNAME pl_gui)
SET(APPBIN pl_gui.wt)
SET(APPRESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/icons ${CMAKE_CURRENT_SOURCE_DIR}/*.xml ${CMAKE_CURRENT_SOURCE_DIR}/*.css ${CMAKE_CURRENT_SOURCE_DIR}/conf/")
CONFIGURE_FILE(
  ${WT_SOURCE_DIR}/deploy.sh
  ${CMAKE_CURRENT_BINARY_DIR}/deploy.sh
)

ADD_DEPENDENCIES(pl_gui.wt wt ${EXAMPLES_CONNECTOR})
