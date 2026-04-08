/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls

import Governikus.TitleBar
import Governikus.Global
import Governikus.Type

Item {
	id: root

	property bool contentIsScrolled: false
	property bool preventScreenshots: false
	property ProgressTracker progress: null
	property bool skipFocusUpdate: false

	signal activate
	signal leaveView

	function updateFocus() {
		if (!visible || skipFocusUpdate) {
			return;
		}
		if (d.forceFocusFirstA11yItem(root)) {
			return;
		}
		let menuBar = (ApplicationWindow.menuBar as TitleBar);
		if (menuBar && menuBar.setActiveFocus) {
			menuBar.setActiveFocus();
		} else {
			console.warn("No focus item or TitleBar found");
		}
	}

	QtObject {
		id: d

		function forceFocusFirstA11yItem(view) {
			if (!view.visible || !ApplicationModel.screenReaderRunning) {
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
