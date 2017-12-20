ADD_QML_TEST_FILES(${RESOURCES_DIR}/qml)

ADD_EXECUTABLE(QmlTestRunner QmlTestRunner.cpp)
TARGET_LINK_LIBRARIES(QmlTestRunner Qt5::QuickTest Qt5::Gui)
