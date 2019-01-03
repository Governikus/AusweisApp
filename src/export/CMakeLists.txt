#####################################################################
# The module export is responsible to export certain data to other
# file formats.
# Supported: PDF
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppExport)

TARGET_LINK_LIBRARIES(AusweisAppExport Qt5::Core Qt5::Svg AusweisAppCard AusweisAppSettings)
