#####################################################################
# The module contains the interface for the EidAppletService
#####################################################################

if(ANDROID)
	file(GLOB EXTERNAL_SMART_FILES *.cpp)
else()
	file(GLOB EXTERNAL_SMART_FILES mock/*.cpp)
endif()

add_library(AusweisAppExternalSmartID ${EXTERNAL_SMART_FILES})
target_include_directories(AusweisAppExternalSmartID INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
target_link_libraries(AusweisAppExternalSmartID AusweisAppNetwork)

if(IOS)
	set_target_properties(AusweisAppExternalSmartID PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

add_library(AusweisAppExternal::SmartID ALIAS AusweisAppExternalSmartID)
