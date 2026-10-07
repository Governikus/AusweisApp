/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
 */

import QtQuick

import Governikus.Type

Controller {
	id: root

	property bool enableTileStyle: true
	property bool lockAndHideNavigation: false
	property var navigationAction: null
	property Component rightTitleBarAction: null
	property bool showTitleBarContent: true
	required property string title

	Connections {
		function onActivate() {
			if (ApplicationModel.screenReaderRunning) {
				root.updateFocus();
			}
		}
	}
}
