/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ApplicationModel 1.0


Button {
	id: control

	property int buttonType // Qt.ForwardButton || Qt.BackButton

	width: ApplicationModel.scaleFactor * 160
	height: ApplicationModel.scaleFactor * 160

	Accessible.role: Accessible.Button
	//: LABEL DESKTOP_QML
	Accessible.name: {
		if (buttonType === Qt.ForwardButton) {
			return qsTr("Continue") + SettingsModel.translationTrigger
		}
		console.assert(buttonType === Qt.BackButton, "Use either Qt.ForwardButton or Qt.BackButton")
		return qsTr("Back") + SettingsModel.translationTrigger
	}

	background: Rectangle {
		anchors.fill: parent
		anchors.margins: control.down ? control.height / 32 : 0

		radius: height / 2
		border.width: height / 40;
		border.color: enabled ? Constants.white : Constants.red
		color: Style.color.transparent

		Rectangle {
			anchors.fill: parent
			anchors.margins: parent.height / 8;

			radius: height / 2
			color: Qt.lighter(Constants.blue, 1.1)

			Image {
				anchors.centerIn: parent

				source: "qrc:///images/desktop/continue_arrow.svg"
				sourceSize.height: parent.height / 2;
				transformOrigin: Item.Center
				rotation: buttonType === Qt.BackButton ? 180 : 0
			}
		}
	}

	FocusFrame {}
}
