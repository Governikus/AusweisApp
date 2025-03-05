/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
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
	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (root.smartState) {
			case SmartModel.State.UPDATING_STATUS:
				//: LABEL ANDROID IOS
				return qsTr("Updating Smart-eID status...");
			case SmartModel.State.READY:
				//: LABEL ANDROID IOS
				return qsTr("Smart-eID ready for use");
			default:
				return "";
			}
		}
		textStyle: Style.text.headline
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
				//: LABEL ANDROID IOS
				return qsTr("Please wait a moment.");
			case SmartModel.State.READY:
				//: LABEL ANDROID IOS
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

			//: LABEL ANDROID IOS
			text: qsTr("With the Smart-eID you may also use the online identification function without the ID card.")
			width: parent.width
		}
		GText {

			//: LABEL ANDROID IOS
			text: qsTr("Check here if your device is suitable to set up a Smart-eID.")
			width: parent.width
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GButton {
		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.pane_spacing
		icon.source: "qrc:///images/device_button.svg"
		//: LABEL ANDROID IOS
		text: qsTr("Start check")
		tintIcon: true
		visible: root.showCheck

		onClicked: root.showCheckResult()
	}
}
