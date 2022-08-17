/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.TitleBar 1.0
import Governikus.Style 1.0
import Governikus.View 1.0

Controller {
	id: controller

	function updateTitleBarActions() {
		let menuBar = ApplicationWindow.menuBar
		if (menuBar && menuBar.updateActions) {
			menuBar.updateActions()
		} else {
			console.warn("Cannot update actions, ApplicationWindow.menuBar.updateActions() is undefined")
		}
	}

	property bool isAbstract: false
	property TitleBarAction titleBarAction: null
	readonly property bool sectionPageTypeMarker: true

	function setActive() {
		updateFocus()
	}

	anchors.fill: parent

	onVisibleChanged: setActive()
}
