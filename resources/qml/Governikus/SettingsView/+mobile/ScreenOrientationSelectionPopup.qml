/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

ConfirmationPopup {
	style: ConfirmationPopup.PopupStyle.OkButton
	//: LABEL ANDROID
	title: qsTr("Select screen orientation")

	onConfirmed: {
		if (phone.checked && plugin.isTabletLayout) {
			plugin.applyPlatformStyle("mobile,phone," + Qt.platform.os);
		}
		if (tablet.checked && !plugin.isTabletLayout) {
			plugin.applyPlatformStyle("mobile,tablet," + Qt.platform.os);
		}
	}

	Column {
		spacing: Constants.component_spacing
		width: parent.width

		GRadioButton {
			id: phone

			//: LABEL ANDROID
			Accessible.description: qsTr("Set screen orientation to portrait")
			checked: !plugin.isTabletLayout
			icon.source: "qrc:///images/android/stay_primary_portrait-24px.svg"
			//: LABEL ANDROID
			text: qsTr("Portrait") + (plugin.tablet ? "" : (" (%1)".arg(qsTr("recommended"))))
			tintIcon: true
			width: parent.width
		}
		GRadioButton {
			id: tablet

			//: LABEL ANDROID
			Accessible.description: qsTr("Set screen orientation to landscape")
			checked: plugin.isTabletLayout
			icon.source: "qrc:///images/android/stay_primary_landscape-24px.svg"
			//: LABEL ANDROID
			text: qsTr("Landscape") + (plugin.tablet ? (" (%1)".arg(qsTr("recommended"))) : "")
			tintIcon: true
			width: parent.width
		}
		GText {

			//: LABEL ANDROID
			text: qsTr("Using a screen orientation unfit for your device may result in display errors.")
			textStyle: Style.text.normal_warning
			visible: tablet.checked && !plugin.tablet
			width: parent.width
		}
	}
}
