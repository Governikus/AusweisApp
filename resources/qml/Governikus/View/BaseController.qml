/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Type.ApplicationModel

Item {
	id: root

	function showRemoveCardFeedback(workflowModel, success) {
		if (workflowModel.showRemoveCardFeedback) {
			workflowModel.showRemoveCardFeedback = false;
			if (Qt.platform.os === "ios") {
				// The feedback notification will crash Apple's VoiceOver if it happens at the same time the app is redirecting
				// back to the browser. This happens with both the iOS toasts and our own toast-like replacement. To work around
				// this, we will not show the notification during an authentication on iOS with VoiceOver running.
				if (ApplicationModel.isScreenReaderRunning() && ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_AUTHENTICATION) {
					return;
				}
			}
			if (success) {
				//: INFO ALL_PLATFORMS The workflow finished successfully, the ID card may (and should) be removed from the card reader.
				ApplicationModel.showFeedback(qsTr("Process finished successfully. You may now remove your ID card from the device."));
			} else {
				//: INFO ALL_PLATFORMS The workflow is completed, the ID card may (and should) be removed from the card reader.
				ApplicationModel.showFeedback(qsTr("You may now remove your ID card from the device."));
			}
		}
	}
	function updateFocus() {
		if (!visible) {
			return;
		}
		if (d.forceFocusFirstA11yItem(root)) {
			return;
		}
		let menuBar = ApplicationWindow.menuBar;
		if (menuBar && menuBar.setActiveFocus) {
			console.warn("No focus item found using TitleBar");
			menuBar.setActiveFocus();
		} else {
			console.warn("No focus item or TitleBar found");
		}
	}

	QtObject {
		id: d

		function forceFocusFirstA11yItem(view) {
			if (!view.visible) {
				return false;
			}
			let isA11yFocusable = view.Accessible && view.Accessible.focusable && !view.Accessible.ignored;
			if (isA11yFocusable) {
				view.forceActiveFocus(Qt.MouseFocusReason);
				return true;
			}
			for (let i = 0; i < view.children.length; i++) {
				let child = view.children[i];
				if (forceFocusFirstA11yItem(child)) {
					return true;
				}
			}
			return false;
		}
	}
}
