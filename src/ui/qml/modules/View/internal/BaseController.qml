/**
 * Copyright (c) 2021-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Controls

import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type

Item {
	id: root

	property bool contentIsScrolled: false
	property Item lastA11yFocusedItem: null
	property bool lockAndHideNavigation: false
	property bool preventScreenshots: false
	property ProgressTracker progress: null
	property bool skipInitialFocusUpdate: false

	signal activate
	signal leaveView

	function updateFocus() {
		if (!visible) {
			return;
		}
		if (skipInitialFocusUpdate && !lastA11yFocusedItem) {
			return;
		}
		if (d.forceFocusRestoreA11y()) {
			return;
		}
		if (d.forceFocusFirstA11yItem(root)) {
			return;
		}
		if (Style.is_layout_desktop) {
			let menuBar = (ApplicationWindow.menuBar as TitleBar);
			if (menuBar && menuBar.setActiveFocus) {
				menuBar.setActiveFocus();
			} else {
				console.warn("No focus item or TitleBar found");
			}
		} else {
			forceActiveFocus(Qt.MouseFocusReason);
		}
	}

	QtObject {
		id: d

		function forceFocusFirstA11yItem(view) {
			if (!view.visible || !ApplicationModel.screenReaderRunning) {
				return false;
			}
			let isA11yFocusable = !Utils.isAccessibleIgnored(view) && view.Accessible.focusable;
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
		function forceFocusRestoreA11y() {
			if (!ApplicationModel.screenReaderRunning || !root.lastA11yFocusedItem) {
				return false;
			}
			root.lastA11yFocusedItem.forceActiveFocus(Qt.MouseFocusReason);
			root.lastA11yFocusedItem = null;
			return true;
		}
	}
}
