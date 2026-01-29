/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Type

Item {
	id: root

	property bool resetState: false

	function notifyScreenRecording() {
		if (SettingsModel.screenPrivacy && ApplicationModel.screenRecording) {
			//: ANDROID
			ApplicationModel.showFeedback(qsTr("Attention: Screen Recording in progress"));
		}
	}

	Component.onCompleted: {
		if (SettingsModel.screenPrivacy) {
			ApplicationModel.preventScreenshot(true);
			resetState = true;
		}
		notifyScreenRecording();
	}
	onVisibleChanged: {
		if (visible) {
			if (SettingsModel.screenPrivacy) {
				ApplicationModel.preventScreenshot(true);
				resetState = true;
			}
			notifyScreenRecording();
		}
		if (!visible && resetState) {
			ApplicationModel.preventScreenshot(false);
		}
	}

	Connections {
		function onFireScreenshotTaken() {
			//: IOS
			ApplicationModel.showFeedback(qsTr("Attention: Screenshot taken"));
		}

		enabled: root.visible
		target: ApplicationModel
	}
}
