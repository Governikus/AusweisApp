/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.TitleBar
import Governikus.Style

BaseOnboardingView {
	id: root

	signal useSmartPhone
	signal useUsbReader

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Action.Back
		startEnabled: false

		onNavigationActionClicked: root.leaveView()
	}

	Heading {
		//: DESKTOP
		text: qsTr("How would you like to read your ID card?")
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GInformativeButton {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		Layout.fillWidth: true

		//: DESKTOP
		description: qsTr("Connect an USB card reader")
		isPane: true

		//: DESKTOP
		text: qsTr("By card reader")

		onClicked: root.useUsbReader()
	}
	GInformativeButton {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true

		//: DESKTOP
		description: qsTr("Use a NFC-enabled smartphone as card reader")
		isPane: true

		//: DESKTOP
		text: qsTr("By smartphone")

		onClicked: root.useSmartPhone()
	}
	GSpacer {
		Layout.fillHeight: true
	}
}
