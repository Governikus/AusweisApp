/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.View

FlickableSectionPage {
	id: root

	signal continueClicked

	Heading {
		Layout.bottomMargin: Style.dimens.pane_spacing
		//: ALL_PLATFORMS
		text: qsTr("5-digit Transport PIN")
	}
	AnimationLoader {
		id: animation

		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		animated: false
		type: AnimationLoader.Type.TRANSPORT_PIN
	}
	Subheading {
		//: ALL_PLATFORMS
		text: qsTr("Have your documents ready")
	}
	GText {
		Layout.bottomMargin: Style.dimens.text_spacing
		//: ALL_PLATFORMS
		text: qsTr("You need:")
	}
	BulletPointText {
		//: ALL_PLATFORMS
		text: qsTr("Your %1ID card%2").arg("<b>").arg("</b>")
	}
	BulletPointText {
		Layout.bottomMargin: Style.dimens.pane_spacing * 2
		//: ALL_PLATFORMS
		text: qsTr("Letter with 5-digit %1Transport PIN%2").arg("<b>").arg("</b>")
	}
	Hint {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: (Qt.platform.os === "ios" || Qt.platform.os === "android") ? Style.dimens.pane_spacing : Style.dimens.pane_spacing * 2
		Layout.fillWidth: true
		text: "%1\n\n%2"
		//: ALL_PLATFORMS
		.arg(qsTr("You have to enter the Transport PIN only when activating the ID card for the first time. Your own card PIN is linked on all devices with your ID card as soon as you have set it."))
		//: ALL_PLATFORMS
		.arg(qsTr("In this case, tap on Back and choose 6-digit PIN."))
		//: ALL_PLATFORMS
		title: qsTr("You are not using the ID card online for the 1st time?")
	}
	GContinueButton {
		onClicked: root.continueClicked()
	}
}
