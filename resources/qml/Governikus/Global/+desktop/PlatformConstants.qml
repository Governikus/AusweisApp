/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Type.ApplicationModel 1.0

BrandConstants {
	readonly property color blue: "#5481ab"

	readonly property int component_spacing: ApplicationModel.scaleFactor * 30
	readonly property int pane_padding: ApplicationModel.scaleFactor * 30
	readonly property int pane_spacing: ApplicationModel.scaleFactor * 30
	readonly property int groupbox_spacing: ApplicationModel.scaleFactor * 20
	readonly property int text_spacing: ApplicationModel.scaleFactor * 10

	readonly property bool is_desktop: true
}
