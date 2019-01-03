import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.Type.RemoteServiceModel 1.0

Rectangle {
	id: baseItem
	height: sectionRow.height
	color: Constants.background_color

	property bool showStartStopButton: true
	property string selectedSection: showStartStopButton ? "STARTSTOP" : "REMOTE"

	Row {
		id: sectionRow
		spacing: Utils.dp(30)
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter

		TechnologySwitchButton {
			visible: showStartStopButton
			buttonActive: selectedSection !== "STARTSTOP"
			onClicked: selectedSection = "STARTSTOP"
			imageSource: "qrc:///images/icon_remote.svg"
			text: qsTr("Service") + settingsModel.translationTrigger
		}

		TechnologySwitchButton {
			buttonActive: selectedSection !== "REMOTE"
			onClicked: selectedSection = "REMOTE"
			imageSource: "qrc:///images/icon_pair.svg"
			text: qsTr("Pairing") + settingsModel.translationTrigger
		}

		TechnologySwitchButton {
			buttonActive: selectedSection !== "LOCAL"
			onClicked: selectedSection = "LOCAL"
			imageSource: "qrc:///images/icon_settings.svg"
			text: qsTr("Settings") + settingsModel.translationTrigger
		}
	}
}
