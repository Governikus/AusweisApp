/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

import QtQuick

import Governikus.Type

PlatformTextField {
	id: root

	// iOS prioritizes manual announcements and kills previous ones like the content announcement
	onActiveFocusChanged: Qt.platform.os !== "ios" && root.activeFocus && d.announceMaxReached()
	onTextChanged: d.announceMaxReached()

	GToolTip {
		text: d.maxReachedText
		visible: !ApplicationModel.screenReaderRunning && d.maxReached(root.text)
	}
	QtObject {
		id: d

		//: ALL_PLATFORMS
		readonly property string maxReachedText: qsTr("Maximum allowed length reached.")

		function announceMaxReached() {
			if (ApplicationModel.screenReaderRunning && d.maxReached(root.text))
				root.Accessible.announce(d.maxReachedText);
		}
		function maxReached(pText) {
			return pText.length === root.maximumLength;
		}
	}
}
