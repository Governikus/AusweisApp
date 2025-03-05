/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.TitleBar

BaseOnboardingView {
	id: root

	signal setupDesktopUsage

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.leaveView()
	}

	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter

		//: LABEL ANDROID IOS
		text: qsTr("Do you want to also use the %1 on a PC/Mac?").arg(Qt.application.name)
		textStyle: Style.text.headline
		wrapMode: Text.WordWrap
	}
	GText {
		Layout.topMargin: Style.dimens.pane_spacing
		//: LABEL ANDROID IOS
		text: qsTr("In order to also perform online authentications on a PC/Mac, you may pair your smartphone as a card reader. To do so, you need to have the %1 installed on both devices.").arg(Qt.application.name)
	}
	GInformativeButton {
		Layout.fillWidth: true
		Layout.topMargin: Style.dimens.pane_spacing
		isPane: true
		//: LABEL ANDROID IOS
		text: qsTr("No, I just want to use this device")

		onClicked: root.continueOnboarding()
	}
	GInformativeButton {
		Layout.fillWidth: true
		Layout.topMargin: Style.dimens.pane_spacing
		isPane: true
		//: LABEL ANDROID IOS
		text: qsTr("Yes, I also want to use a PC/Mac")

		onClicked: root.setupDesktopUsage()
	}
}
