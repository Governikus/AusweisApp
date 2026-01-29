/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
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

	Heading {
		Layout.bottomMargin: Style.dimens.pane_spacing
		//: MOBILE
		text: qsTr("Set up %1 on the PC/Mac").arg(Qt.application.name)
	}
	TintableIcon {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		source: "qrc:///images/phone_to_pc.svg"
		sourceSize.height: Style.dimens.medium_icon_size
		tintColor: Style.color.image
	}
	Subheading {
		//: MOBILE
		text: "1. " + qsTr("Install %1").arg(Qt.application.name)
	}
	GText {
		//: MOBILE %1 is replaced with the name "AusweisApp"
		text: qsTr("In order to install the %1, open the following website on your PC/Mac:").arg(Qt.application.name)
	}
	GText {
		readonly property string downloadLink: "https://www.ausweisapp.bund.de/%1/aa2/download".arg(SettingsModel.language)

		Layout.bottomMargin: Style.dimens.pane_spacing
		text: "<a href=\"%1\">%1</a>".arg(downloadLink)
	}
	Subheading {
		//: MOBILE %1 is replaced with the name "AusweisApp"
		text: "2. " + qsTr("Open %1").arg(Qt.application.name)
	}
	GText {
		Layout.bottomMargin: Style.dimens.pane_spacing

		//: MOBILE %1 is replaced with the name "AusweisApp"
		text: qsTr("Open the %1 on your PC/Mac.").arg(Qt.application.name)
	}
	Subheading {
		//: MOBILE
		text: "3. " + qsTr("Follow the instruction on your second device")
	}
	GText {
		Layout.bottomMargin: Style.dimens.pane_spacing

		//: MOBILE
		text: qsTr("Now follow the instruction for the setup on your PC/Mac. If the setup does not start automatically, you may find it under Help > Setup.")
	}
	GContinueButton {
		//: MOBILE
		text: qsTr("Continue with pairing")

		onClicked: root.continueToPairing()
	}
}
