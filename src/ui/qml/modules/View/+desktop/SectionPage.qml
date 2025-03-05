/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls
import QtQml

import Governikus.TitleBar
import Governikus.View
import Governikus.Workflow

BaseController {
	id: root

	property bool smartEidUsed: false
	readonly property var stackView: StackView.view
	required property string title
	property TitleBarSettings titleBarSettings: TitleBarSettings {
	}
	readonly property bool workflowActive: stackView ? (stackView.currentItem instanceof GeneralWorkflow) : false

	function pop(pItem) {
		if (stackView) {
			stackView.pop(pItem);
		} else {
			console.log("Controller not attached to StackView");
		}
	}
	function popAll() {
		if (stackView) {
			stackView.pop(null);
		} else {
			console.log("Controller not attached to StackView");
		}
	}
	function push(pSectionPage, pProperties) {
		if (stackView) {
			if (pSectionPage === stackView.currentItem) {
				return;
			}
			stackView.push(pSectionPage, pProperties);
		} else {
			console.log("Controller not attached to StackView");
		}
	}
	function replace(pSectionPage, pProperties) {
		if (stackView) {
			if (pSectionPage === stackView.currentItem) {
				return;
			}
			if (stackView.depth <= 1) {
				stackView.push(pSectionPage, pProperties);
				return;
			}
			stackView.replace(pSectionPage, pProperties);
		} else {
			console.log("Controller not attached to StackView");
		}
	}
	function setActive() {
		timer.start();
	}

	Component.onCompleted: setActive()
	Keys.onEscapePressed: event => {
		if (titleBarSettings.navigationAction === NavigationAction.Back && titleBarSettings.navigationEnabled) {
			titleBarSettings.navigationActionClicked();
		} else {
			event.accepted = false;
		}
	}
	onVisibleChanged: setActive()

	Timer {
		id: timer

		interval: 0

		onTriggered: root.updateFocus()
	}
}
