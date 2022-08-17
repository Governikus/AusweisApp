ADD_QML_TEST_FILES()

set(MAIN_FILES QmlTestRunner.cpp)
if(ANDROID)
	add_library(QmlTestRunner SHARED ${MAIN_FILES})
else()
	add_executable(QmlTestRunner ${MAIN_FILES})
endif()

target_link_libraries(QmlTestRunner ${Qt}::QuickTest ${Qt}::Gui AusweisAppGlobal AusweisAppUiQml AusweisAppTestHelper)
if(QT6 AND WIN32)
	ADD_SHADERS_TO_TARGET(QmlTestRunner)
endif()

if(TARGET AusweisAppRcc)
	add_dependencies(QmlTestRunner AusweisAppRcc)
	add_custom_command(TARGET QmlTestRunner POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RCC}" "$<TARGET_FILE_DIR:QmlTestRunner>")
endif()
