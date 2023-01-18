/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.TitleBar 1.0
import Governikus.Style 1.0
import Governikus.View 1.0

Controller {
	id: controller

	property bool isAbstract: false
	readonly property bool sectionPageTypeMarker: true
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
