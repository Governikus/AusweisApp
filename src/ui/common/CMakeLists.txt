#####################################################################
# Common UI functions used by multiple UI implementations
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiCommon)

TARGET_LINK_LIBRARIES(AusweisAppUiCommon Qt5::Core Qt5::Gui AusweisAppGlobal)

IF(DESKTOP)
	TARGET_LINK_LIBRARIES(AusweisAppUiCommon Qt5::Widgets)
ENDIF()

IF(MAC)
	TARGET_LINK_LIBRARIES(AusweisAppUiCommon ${OSX_APPKIT})
ENDIF()
