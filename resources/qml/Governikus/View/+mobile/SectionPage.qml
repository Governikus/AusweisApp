/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type.ApplicationModel

Controller {
	id: root

	property bool contentIsScrolled: false
	property bool hiddenNavbarPadding: false
	property var navigationAction: null
	property var rightTitleBarAction: null
	property bool smartEidUsed: false
	required property string title
	property real titleBarOpacity: 1
	property bool titleBarVisible: true

	signal activate
	signal reset

	Connections {
		function onActivate() {
			if (ApplicationModel.isScreenReaderRunning()) {
				updateFocus();
			}
		}
	}
}
