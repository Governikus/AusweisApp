/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type

Controller {
	id: root

	property bool enableTileStyle: true
	property var navigationAction: null
	property var rightTitleBarAction: null
	property bool showTitleBarContent: true
	property bool smartEidUsed: false
	required property string title

	clip: true

	Connections {
		function onActivate() {
			if (ApplicationModel.isScreenReaderRunning()) {
				updateFocus();
			}
		}
	}
}
