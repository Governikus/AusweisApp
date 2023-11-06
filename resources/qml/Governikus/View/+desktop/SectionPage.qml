/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.TitleBar
import Governikus.Style
import Governikus.View

Controller {
	id: controller

	readonly property bool breadcrumpSearchPath: true
	property bool isAbstract: false
	property TitleBarAction titleBarAction: null

	function setActive() {
		updateFocus();
	}
	function updateTitleBarActions() {
		let menuBar = ApplicationWindow.menuBar;
		if (menuBar && menuBar.updateActions) {
			menuBar.updateActions();
		} else {
			console.warn("Cannot update actions, ApplicationWindow.menuBar.updateActions() is undefined");
		}
	}

	anchors.fill: parent

	onVisibleChanged: setActive()
}
