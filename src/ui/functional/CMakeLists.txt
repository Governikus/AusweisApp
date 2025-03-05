#####################################################################
# The functional plugin implements the ui interface for callbacks.
#
# This plugin can be used as a SDK that will be linked into another
# binary.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiFunctional)

target_link_libraries(AusweisAppUiFunctional ${Qt}::Core AusweisAppUi AusweisAppUiJson AusweisAppGlobal AusweisAppInit)
target_compile_definitions(AusweisAppUiFunctional PRIVATE QT_STATICPLUGIN)

SET_PUBLIC_HEADER(AusweisAppUiFunctional AusweisApp.h AusweisApp2.h)

if(IOS)
	target_link_options(AusweisAppUiFunctional PUBLIC -exported_symbols_list ${CMAKE_CURRENT_SOURCE_DIR}/exported_symbols.txt)
endif()
