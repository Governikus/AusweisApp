/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
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
		navigationAction: NavigationAction.Back
		startEnabled: false

		onNavigationActionClicked: root.leaveView()
	}

	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		//: LABEL DESKTOP
		text: qsTr("How would you like to read your ID card?")
		textStyle: Style.text.headline
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GInformativeButton {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		Layout.fillWidth: true

		//: LABEL DESKTOP
		description: qsTr("Connect an USB card reader")
		isPane: true

		//: LABEL DESKTOP
		text: qsTr("By card reader")

		onClicked: root.useUsbReader()
	}
	GInformativeButton {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true

		//: LABEL DESKTOP
		description: qsTr("Use a NFC-enabled smartphone as card reader")
		isPane: true

		//: LABEL DESKTOP
		text: qsTr("By smartphone")

		onClicked: root.useSmartPhone()
	}
	GSpacer {
		Layout.fillHeight: true
	}
}
