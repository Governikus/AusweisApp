/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Type

Controller {
	id: root

	property bool enableTileStyle: true
	property var navigationAction: null
	property var rightTitleBarAction: null
	property bool showTitleBarContent: true
	property bool smartEidUsed: false
	required property string title

	Connections {
		function onActivate() {
			if (ApplicationModel.isScreenReaderRunning) {
				root.updateFocus();
			}
		}
	}
}
