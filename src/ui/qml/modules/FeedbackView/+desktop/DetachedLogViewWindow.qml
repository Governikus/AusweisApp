/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls

import Governikus.Type

ApplicationWindow {
	id: root

	height: UiPluginModel.initialWindowSize.height
	minimumHeight: UiPluginModel.initialWindowSize.height
	minimumWidth: UiPluginModel.initialWindowSize.width
	title: qsTr("Detached log viewer")
	visible: true
	width: UiPluginModel.initialWindowSize.width

	Shortcut {
		enabled: Qt.platform.os === "osx"
		sequence: "Ctrl+W"

		onActivated: root.close()
	}
	DetachedLogView {
		anchors.fill: parent
		focus: true
	}
}
