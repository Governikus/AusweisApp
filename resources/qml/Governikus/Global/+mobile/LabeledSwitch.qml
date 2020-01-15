/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.0
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	property alias title: titleText.text
	property alias description: descriptionText.text
	property alias checked: entrySwitch.checked

	height: Math.max(textContainer.height, entrySwitch.height)

	Item {
		id: textContainer

		height: titleText.height + descriptionText.height
		anchors.left: parent.left
		anchors.right: entrySwitch.left
		anchors.rightMargin: Constants.component_spacing
		anchors.verticalCenter: parent.verticalCenter

		GText {
			id: titleText

			width: parent.width

			textStyle: Style.text.normal_accent
		}

		GText {
			id: descriptionText

			width: parent.width
			anchors.top: titleText.bottom
			anchors.topMargin: 2

			textStyle: Style.text.normal_secondary
		}
	}

	MouseArea {
		anchors.fill: parent

		onClicked: entrySwitch.toggle()
	}

	GSwitch {
		id: entrySwitch

		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter

		text: titleText.text
	}
}
