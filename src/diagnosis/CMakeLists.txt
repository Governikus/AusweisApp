#####################################################################
# The module diagnosis is responsible to collect and present all
# diagnosis data.
#####################################################################
if(DESKTOP AND TARGET ${Qt}::Qml)
	ADD_PLATFORM_LIBRARY(AusweisAppDiagnosis)

	target_link_libraries(AusweisAppDiagnosis ${Qt}::Qml AusweisAppCard)
	qt_extract_metatypes(AusweisAppDiagnosis) # QTBUG-123574
endif()
