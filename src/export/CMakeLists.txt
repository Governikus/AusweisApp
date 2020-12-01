#####################################################################
# The module export is responsible to export certain data to other
# file formats.
# Supported: PDF
#####################################################################

if(TARGET ${Qt}::Svg)
	ADD_PLATFORM_LIBRARY(AusweisAppExport)

	target_link_libraries(AusweisAppExport ${Qt}::Core ${Qt}::Svg AusweisAppCard AusweisAppSettings)
endif()
