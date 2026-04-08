/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Type

Item {
	id: root

	required property bool preventScreenshots
	property bool resetState: false

	function notifyScreenRecording() {
		if (preventScreenshots && SettingsModel.screenPrivacy && ApplicationModel.screenRecording) {
			//: MOBILE
			ApplicationModel.showFeedback(qsTr("Attention: Screen Recording in progress"));
		}
	}

	onPreventScreenshotsChanged: {
		Qt.callLater(notifyScreenRecording);
		if (preventScreenshots) {
			if (SettingsModel.screenPrivacy) {
				ApplicationModel.preventScreenshot(true);
				resetState = true;
			}
		}
		if (!preventScreenshots && resetState) {
			ApplicationModel.preventScreenshot(false);
		}
	}

	Connections {
		function onFireScreenshotTaken() {
			//: IOS
			ApplicationModel.showFeedback(qsTr("Attention: Screenshot taken"));
		}

		enabled: root.preventScreenshots
		target: ApplicationModel
	}
}
