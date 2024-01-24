ADD_QML_TEST_FILES()

set(MAIN_FILES QmlTestRunner.cpp)
if(ANDROID)
	add_library(QmlTestRunner SHARED ${MAIN_FILES})
else()
	add_executable(QmlTestRunner ${MAIN_FILES})
endif()

target_link_libraries(QmlTestRunner ${Qt}::QuickTest ${Qt}::Gui AusweisAppGlobal AusweisAppUiQml AusweisAppTestHelper)
if(WIN32)
	ADD_SHADERS_TO_TARGET(QmlTestRunner)
endif()

if(CMAKE_GENERATOR MATCHES "Xcode")
	set_target_properties(QmlTestRunner PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE} "${CMAKE_CURRENT_BINARY_DIR}")
endif()

if(TARGET AusweisAppRcc)
	add_custom_target(AusweisAppTestQmlRcc DEPENDS AusweisAppRcc)
	add_custom_command(TARGET AusweisAppTestQmlRcc POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RCC}" "${CMAKE_CURRENT_BINARY_DIR}")
	add_dependencies(QmlTestRunner AusweisAppTestQmlRcc)
endif()
