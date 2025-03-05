/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Init
import Governikus.Workflow
import Governikus.Navigation

BaseController {
	readonly property Navigation navigation: {
		if (ApplicationWindow.window === null) {
			return null;
		}
		if ((ApplicationWindow.window as App) === null) {
			return null;
		}
		return (ApplicationWindow.window as App).navigationInstance;
	}
	property var stackView: {
		if (StackView.view) {
			return StackView.view;
		} else if (parent && (parent as Controller)) {
			return (parent as Controller).stackView;
		} else {
			return parent;
		}
	}
	readonly property bool workflowActive: stackView ? (stackView.currentItem instanceof GeneralWorkflow) : false

	function find(pCallback) {
		if (stackView) {
			return stackView.find(pCallback);
		} else {
			console.log("Controller not attached to StackView");
			return null;
		}
	}
	function getLockedAndHidden() {
		if (navigation && navigation.lockedAndHidden !== undefined) {
			return navigation.lockedAndHidden;
		}
		console.log("Controller cannot find navigation");
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
		if (navigation && navigation.setLockedAndHidden) {
			navigation.setLockedAndHidden(pLockedAndHidden);
		} else {
			console.log("Controller cannot find navigation");
		}
	}
	function show(pModule, pLockedAndHidden = false) {
		if (navigation && navigation.show) {
			navigation.show(pModule, pLockedAndHidden);
		} else {
			console.log("Controller cannot find navigation");
		}
	}
}
