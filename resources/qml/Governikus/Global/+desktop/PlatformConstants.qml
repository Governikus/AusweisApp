/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Type.ApplicationModel 1.0

Item {
	readonly property int component_spacing: ApplicationModel.scaleFactor * 30
	readonly property int pane_padding: ApplicationModel.scaleFactor * 30
	readonly property int pane_spacing: ApplicationModel.scaleFactor * 30
	readonly property int groupbox_spacing: ApplicationModel.scaleFactor * 20
	readonly property int text_spacing: ApplicationModel.scaleFactor * 10

	readonly property bool is_desktop: true
}
