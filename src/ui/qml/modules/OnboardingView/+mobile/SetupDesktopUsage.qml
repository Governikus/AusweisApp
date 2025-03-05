/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type

BaseOnboardingView {
	id: root

	signal continueToPairing

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.leaveView()
	}

	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ANDROID IOS
		text: qsTr("Set up %1 on the PC/Mac").arg(Qt.application.name)
		textStyle: Style.text.headline
	}
	TintableIcon {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		source: "qrc:///images/phone_to_pc.svg"
		sourceSize.height: Style.dimens.medium_icon_size
		tintColor: Style.color.image
	}
	GText {
		//: LABEL ANDROID IOS
		text: "1. " + qsTr("Install %1").arg(Qt.application.name)
		textStyle: Style.text.subline
	}
	GText {
		//: LABEL ANDROID IOS %1 is replaced with the name "AusweisApp"
		text: qsTr("In order to install the %1, open the following website on your PC/Mac:").arg(Qt.application.name)
	}
	GText {
		readonly property string link: "https://www.ausweisapp.bund.de/%1/aa2/download".arg(SettingsModel.language)

		Layout.bottomMargin: Style.dimens.pane_spacing
		text: "<a href=\"%1\">%1</a>".arg(link)
	}
	GText {
		//: LABEL ANDROID IOS %1 is replaced with the name "AusweisApp"
		text: "2. " + qsTr("Open %1").arg(Qt.application.name)
		textStyle: Style.text.subline
	}
	GText {
		Layout.bottomMargin: Style.dimens.pane_spacing

		//: LABEL ANDROID IOS %1 is replaced with the name "AusweisApp"
		text: qsTr("Open the %1 on your PC/Mac.").arg(Qt.application.name)
	}
	GText {
		//: LABEL ANDROID IOS
		text: "3. " + qsTr("Follow the instruction on your second device")
		textStyle: Style.text.subline
	}
	GText {
		Layout.bottomMargin: Style.dimens.pane_spacing

		//: LABEL ANDROID IOS
		text: qsTr("Now follow the instruction for the setup on your PC/Mac. If the onboarding does not start automatically, you may find it under Help > Onboarding.")
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GContinueButton {
		//: LABEL ANDROID IOS
		text: qsTr("Continue with pairing")

		onClicked: root.continueToPairing()
	}
}
