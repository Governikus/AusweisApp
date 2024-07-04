/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Workflow

BaseController {
	readonly property var navBar: typeof (navigation) !== "undefined" ? navigation : parent ? parent.navBar : null
	property var stackView: StackView.view ? StackView.view : (parent && parent.stackView ? parent.stackView : parent)
	readonly property bool workflowActive: stackView ? (stackView.currentItem instanceof GeneralWorkflow) : false

	function getLockedAndHidden() {
		if (navBar && navBar.lockedAndHidden !== undefined) {
			return navBar.lockedAndHidden;
		}
		console.log("Controller cannot find navBar");
		return false;
	}
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
	function setLockedAndHidden(pLockedAndHidden = true) {
		if (navBar && navBar.setLockedAndHidden) {
			navBar.setLockedAndHidden(pLockedAndHidden);
		} else {
			console.log("Controller cannot find navBar");
		}
	}
	function show(pModule, pLockedAndHidden = false) {
		if (navBar && navBar.show) {
			navBar.show(pModule, pLockedAndHidden);
		} else {
			console.log("Controller cannot find navBar");
		}
	}
}
