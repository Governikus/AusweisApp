/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0

Button {
	id: baseItem

	property string subText: ""
	property alias buttonImage: image.source

	height: icon.height + Constants.component_spacing + buttonText.height

	Accessible.role: Accessible.Button
	Accessible.name: subText

	background: Item {
		id: content

		anchors.fill: parent

		Rectangle {
			id: icon

			width: baseItem.width
			height: baseItem.width
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.margins: baseItem.down ? baseItem.width / 32 : 0
			anchors.top: parent.top

			radius: baseItem.width / 2
			border.width: baseItem.width / 40;
			border.color: Constants.white
			color: Style.color.transparent

			Rectangle {
				anchors.fill: parent
				anchors.margins: parent.height / 8;

				radius: height / 2
				color: Qt.lighter(Constants.blue, 1.1)

				Image {
					id: image
					anchors.centerIn: parent

					sourceSize.height: parent.height / 2;
				}
			}
		}

		GText {
			id: buttonText

			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: parent.bottom

			text: baseItem.subText
			textStyle: Style.text.header
		}
	}

	FocusFrame {}
}

