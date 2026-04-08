/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type
import Governikus.Style

ColumnLayout {
	id: root

	signal showUpdateRequested

	spacing: Style.dimens.pane_spacing

	states: [
		State {
			name: "updateAvailable"
			when: d.updateAvailable

			PropertyChanges {
				appcastStatus.text: d.updateData.appcastUpdateText
				showUpdateButton.visible: true
			}
		},
		State {
			name: "automaticSearchEnabled"
			when: SettingsModel.autoUpdateCheck

			PropertyChanges {
				autoUpdateSwitch.drawBottomCorners: true
				updateInfoLayout.visible: false
			}
		},
		State {
			name: "manualSearchRunning"
			when: d.isCheckingForUpdate

			PropertyChanges {
				appcastProgress.visible: true
				//: DESKTOP
				appcastStatus.text: qsTr("Searching for software updates...")
				autoUpdateSwitch.enabled: false
				//: DESKTOP
				searchButton.text: qsTr("Abort search")
			}
		},
		State {
			name: "manualSearchError"
			when: d.updateData.appcastFailed

			PropertyChanges {
				appcastStatus.text: d.updateData.appcastErrorText
			}
		},
		State {
			name: "manualSearchEnabled"
			when: !SettingsModel.autoUpdateCheck

			PropertyChanges {
				appcastStatus.text: d.updateData.appcastNoUpdateText
			}
		}
	]
	transitions: Transition {
		to: "manualSearchRunning"

		SequentialAnimation {
			PauseAnimation {
				duration: Style.animation_duration
			}
			PropertyAction {
				property: "visible"
				target: appcastProgress
			}
			PropertyAction {
				property: "text"
				target: appcastStatus
			}
			PropertyAction {
				property: "enabled"
				target: autoUpdateSwitch
			}
			PropertyAction {
				property: "text"
				target: searchButton
			}
		}
	}

	onHeightChanged: {
		if (height > d.oldHeight && d.scrollPending) {
			d.scrollPending = false;
			Qt.callLater(Utils.positionViewAtItem, updateInfoLayout);
		}
		d.oldHeight = height;
	}

	QtObject {
		id: d

		readonly property bool isCheckingForUpdate: updateData.appcastRunning
		property int oldHeight: 0
		property bool scrollPending: false
		readonly property bool updateAvailable: updateData.updateAvailable
		readonly property var updateData: SettingsModel.appUpdateData
	}
	GSwitch {
		id: autoUpdateSwitch

		checked: SettingsModel.autoUpdateCheck
		//: DESKTOP %1 is replaced with the application name
		description: qsTr("When you restart %1 and every 24 hours, it automatically checks for updates. Updates are not performed automatically. If this option is disabled, you have to manually check for updates in the settings.").arg(Qt.application.name)
		enabled: !SettingsModel.autoUpdateCheckSetByAdmin

		//: DESKTOP
		text: qsTr("Automatically check for software updates at program start (recommended)")

		onCheckedChanged: {
			if (checked === SettingsModel.autoUpdateCheck)
				return;

			if (!checked && d.isCheckingForUpdate)
				d.updateData.abortDownload();
			SettingsModel.autoUpdateCheck = checked;
			if (checked && !d.isCheckingForUpdate)
				SettingsModel.updateAppcast();
			d.scrollPending = true;
		}
	}
	ColumnLayout {
		id: updateInfoLayout

		Layout.bottomMargin: Style.dimens.pane_padding
		Layout.leftMargin: Style.dimens.pane_padding
		Layout.rightMargin: Style.dimens.pane_padding
		spacing: root.spacing

		GText {
			id: appcastStatus

			visible: text !== ""
		}
		ColumnLayout {
			spacing: root.spacing
			visible: !SettingsModel.autoUpdateCheck && !d.updateAvailable

			GProgressBar {
				id: appcastProgress

				text: "%1 %".arg(Math.floor(value))
				value: 100 * d.updateData.appcastProgress / d.updateData.appcastTotal
				visible: false
			}
			GLink {
				id: searchButton

				colorStyle: Style.color.linkTitle
				font.underline: true
				horizontalPadding: 0
				//: DESKTOP
				text: qsTr("Start manual search for software update")
				verticalPadding: 0

				onClicked: !d.isCheckingForUpdate ? SettingsModel.updateAppcast() : d.updateData.abortDownload()
			}
		}
		GButton {
			id: showUpdateButton

			//: DESKTOP
			text: qsTr("Show update")
			visible: false

			onClicked: root.showUpdateRequested()
		}
	}
}
