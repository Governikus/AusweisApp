/**
 * Copyright (c) 2021-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Controls

import Governikus.Init
import Governikus.Navigation
import Governikus.Type
import Governikus.Workflow

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
			if (ApplicationModel.screenReaderRunning) {
				lastA11yFocusedItem = Window.activeFocusItem;
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
	function show(pModule) {
		if (navigation && navigation.show) {
			navigation.show(pModule);
		} else {
			console.log("Controller cannot find navigation");
		}
	}
}
