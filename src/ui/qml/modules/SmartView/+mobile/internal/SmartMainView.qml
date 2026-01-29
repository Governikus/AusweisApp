/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

ColumnLayout {
	id: root

	readonly property bool showCheck: smartState !== SmartModel.State.UPDATING_STATUS && smartState !== SmartModel.State.READY
	readonly property int smartState: SmartModel.state

	signal changePin
	signal deletePersonalization
	signal showCheckResult
	signal startSelfAuth
	signal updateSmart

	spacing: 0

	SmartCardView {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		Layout.maximumHeight: Style.dimens.header_icon_size
	}
	Heading {
		text: {
			switch (root.smartState) {
			case SmartModel.State.UPDATING_STATUS:
				//: MOBILE
				return qsTr("Updating Smart-eID status...");
			case SmartModel.State.READY:
				//: MOBILE
				return qsTr("Smart-eID ready for use");
			default:
				return "";
			}
		}
		visible: !root.showCheck
		wrapMode: Text.WordWrap
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.text_spacing
		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (root.smartState) {
			case SmartModel.State.UPDATING_STATUS:
				//: MOBILE
				return qsTr("Please wait a moment.");
			case SmartModel.State.READY:
				//: MOBILE
				return qsTr("Your Smart-eID is set up and ready for use. You can now perform online identifications without your ID card if supported by the provider.");
			default:
				return "";
			}
		}
		visible: !root.showCheck
		wrapMode: Text.WordWrap
	}
	SmartSettingsView {
		Layout.topMargin: Style.dimens.pane_spacing
		visible: root.smartState === SmartModel.State.READY

		onChangePin: root.changePin()
		onDeletePersonalization: root.deletePersonalization()
		onStartSelfAuth: root.startSelfAuth()
		onUpdateSmart: root.updateSmart()
	}
	GPane {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		visible: root.showCheck

		GText {
			//: MOBILE
			text: qsTr("With the Smart-eID you may also use the online identification function without the ID card.")
		}
		GText {
			//: MOBILE
			text: qsTr("Check here if your device is suitable to set up a Smart-eID.")
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GButton {
		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.pane_spacing
		icon.source: "qrc:///images/device_button.svg"
		//: MOBILE
		text: qsTr("Start check")
		tintIcon: true
		visible: root.showCheck

		onClicked: root.showCheckResult()
	}
}
