/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
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

	Keys.onEnterPressed: root.continueClicked()
	Keys.onEscapePressed: root.leaveView()
	Keys.onReturnPressed: root.continueClicked()

	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ALL_PLATFORMS
		text: qsTr("5-digit Transport PIN")
		textStyle: Style.text.headline
	}
	AnimationLoader {
		id: animation

		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		animated: false
		type: AnimationLoader.TRANSPORT_PIN
	}
	GText {
		//: LABEL ALL_PLATFORMS
		text: qsTr("Have your documents ready")
		textStyle: Style.text.subline
	}
	GText {
		Layout.bottomMargin: Style.dimens.text_spacing
		//: LABEL ALL_PLATFORMS
		text: qsTr("You need:")
	}
	BulletPointText {
		//: LABEL ALL_PLATFORMS
		text: qsTr("Your %1ID card%2").arg("<b>").arg("</b>")
	}
	BulletPointText {
		Layout.bottomMargin: Style.dimens.pane_spacing * 2
		//: LABEL ALL_PLATFORMS
		text: qsTr("Letter with 5-digit %1Transport PIN%2").arg("<b>").arg("</b>")
	}
	Hint {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing * 2
		Layout.fillWidth: true
		text: "%1\n\n%2"
		//: LABEL ALL_PLATFORMS
		.arg(qsTr("You have to enter the Transport PIN only when activating the ID card for the first time. Your own card PIN is linked on all devices with your ID card as soon as you have set it."))
		//: LABEL ALL_PLATFORMS
		.arg(qsTr("In this case, tap on Back and choose 6-digit PIN."))
		//: LABEL ALL_PLATFORMS
		title: qsTr("You are not using the ID card online for the 1st time?")
	}
	GContinueButton {
		onClicked: root.continueClicked()
	}
}
