/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.RemoteServiceModel 1.0

ConfirmationPopup {
	id: popup

	property alias pin: name.text
	property var deviceId

	style: ConfirmationPopup.PopupStyle.NoButtons
	//: INFO ANDROID IOS Header of the pairing dialog when using the smartphone as card reader
	title: qsTr("Pairing code") + SettingsModel.translationTrigger
	//: INFO ANDROID IOS Main text of the pairing dialog when using the smartphone as card reader.
	text: qsTr("Enter this code on your other device to use this device as a card reader") + SettingsModel.translationTrigger

	TextField {
		id: name

		width: parent.width

		Accessible.role: Accessible.StaticText
		Accessible.name: displayText
		Accessible.description: qsTr("Click to close dialog") + SettingsModel.translationTrigger
		Accessible.onPressAction: if (Qt.platform.os === "ios") popup.close()

		focus: true
		horizontalAlignment: Text.AlignHCenter
		font.letterSpacing: 5
		font.pixelSize: 50
		font.bold: true
		readOnly: true
	}
}
