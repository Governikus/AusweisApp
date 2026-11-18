/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
 */

import QtQuick

Controller {
	id: root

	property bool enableTileStyle: true
	property var navigationAction: null
	property Component rightTitleBarAction: null
	property bool showTitleBarContent: true
	required property string title

	Connections {
		function onActivate() {
			Qt.callLater(root.updateFocus);
		}
	}
}
