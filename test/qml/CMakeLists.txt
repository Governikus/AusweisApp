ADD_QML_TEST_FILES()

set(MAIN_FILES QmlTestRunner.cpp)
if(ANDROID)
	add_library(QmlTestRunner SHARED ${MAIN_FILES})
else()
	add_executable(QmlTestRunner ${MAIN_FILES})
endif()

target_link_libraries(QmlTestRunner ${Qt}::QuickTest ${Qt}::Gui AusweisAppGlobal AusweisAppUiQml)
