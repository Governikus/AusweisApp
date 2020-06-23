/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ChangePinModel 1.0


Item {
	readonly property int spacing: (height - pinIcon.height - pinHeader.height - pinDesc.height - govButton.height - 2 * Constants.pane_padding) / 3

	Image {
		id: pinIcon

		height: parent.height * 0.25
		width: height

		anchors.top: parent.top
		anchors.topMargin: spacing
		anchors.horizontalCenter: parent.horizontalCenter

		fillMode: Image.PreserveAspectFit
		smooth: true
		source: "qrc:///images/icon_Pin.svg"
	}

	GText {
		id: pinHeader

		anchors.top: pinIcon.bottom
		anchors.topMargin: spacing
		anchors.horizontalCenter: parent.horizontalCenter

		//: LABEL ANDROID IOS
		text: qsTr("PIN Management") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent
		visible: Constants.is_layout_android
	}

	GText {
		id: pinDesc

		anchors.margins: Constants.pane_padding
		anchors.top: pinHeader.bottom
		anchors.horizontalCenter: parent.horizontalCenter

		//: LABEL ANDROID IOS
		text: qsTr("You have the opportunity to change your Transport PIN into a personal PIN. You can also change the PIN at any time or unblock the PIN using the Personal Unblocking Key (PUK). The Transport PIN and the PUK can be found in the letter sent to you by your competent authority.") + SettingsModel.translationTrigger

		horizontalAlignment: Text.AlignHCenter
		width: Math.min(parent.width - 2 * Constants.pane_padding, Style.dimens.max_text_width)
	}

	GButton {
		id: govButton

		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottomMargin: Constants.pane_padding

		//: LABEL ANDROID IOS
		text: qsTr("Change PIN now") + SettingsModel.translationTrigger
		onClicked: ChangePinModel.startWorkflow()
	}
}
