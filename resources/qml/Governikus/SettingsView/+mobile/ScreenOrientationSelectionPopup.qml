/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0

ConfirmationPopup {
	//: LABEL ANDROID
	title: qsTr("Select screen orientation")

	style: ConfirmationPopup.PopupStyle.OkButton
	onConfirmed: {
		if (phone.checked && Screen.primaryOrientation === Qt.LandscapeOrientation) {
			plugin.applyPlatformStyle("mobile,phone," + Qt.platform.os)
		}
		if (tablet.checked && Screen.primaryOrientation !== Qt.LandscapeOrientation) {
			plugin.applyPlatformStyle("mobile,tablet," + Qt.platform.os)
		}
	}

	Column {
		width: parent.width

		spacing: Constants.component_spacing

		GRadioButton {
			id: phone

			width: parent.width

			//: LABEL ANDROID
			Accessible.description: qsTr("Set screen orientation to portrait")

			checked: Screen.primaryOrientation !== Qt.LandscapeOrientation
			icon.source: "qrc:///images/android/stay_primary_portrait-24px.svg"
			//: LABEL ANDROID
			text: qsTr("Portrait") + (plugin.tablet ? "" : (" (%1)".arg(qsTr("recommended"))))
			tintIcon: true
		}

		GRadioButton {
			id: tablet

			width: parent.width

			//: LABEL ANDROID
			Accessible.description: qsTr("Set screen orientation to landscape")

			checked: Screen.primaryOrientation === Qt.LandscapeOrientation
			icon.source: "qrc:///images/android/stay_primary_landscape-24px.svg"
			//: LABEL ANDROID
			text: qsTr("Landscape") + (plugin.tablet ? (" (%1)".arg(qsTr("recommended"))) : "")
			tintIcon: true
		}

		GText {
			visible: tablet.checked && !plugin.tablet
			width: parent.width

			//: LABEL ANDROID
			text: qsTr("Using a screen orientation unfit for your device may result in display errors.")
			textStyle: Style.text.normal_warning
		}

	}
}
