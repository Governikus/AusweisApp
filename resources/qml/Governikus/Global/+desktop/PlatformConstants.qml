import QtQuick 2.10

import Governikus.Type.ApplicationModel 1.0

BrandConstants {
	readonly property color blue: Qt.darker(background_color, 1.2)
	readonly property color background_color: "#659bcd"
	readonly property color grey_light: "#bbbbbb"
	readonly property color secondary_text: "#dadada"
	readonly property color accent_color: "#7879b2"

	readonly property int header_font_size: ApplicationModel.scaleFactor * 42
	readonly property int pane_title_font_size: ApplicationModel.scaleFactor * 32
	readonly property int normal_font_size: ApplicationModel.scaleFactor * 26
	readonly property int label_font_size: ApplicationModel.scaleFactor * 20
	readonly property int small_font_size: ApplicationModel.scaleFactor * 18

	readonly property int titlebar_padding: ApplicationModel.scaleFactor * 20
	readonly property int titlebar_spacing: ApplicationModel.scaleFactor * 20
	readonly property int titlebar_font_size: ApplicationModel.scaleFactor * 24

	readonly property int component_spacing: ApplicationModel.scaleFactor * 30
	readonly property int pane_padding: ApplicationModel.scaleFactor * 30
	readonly property int pane_spacing: ApplicationModel.scaleFactor * 30
	readonly property int groupbox_spacing: ApplicationModel.scaleFactor * 20

	readonly property bool is_desktop: true
}
