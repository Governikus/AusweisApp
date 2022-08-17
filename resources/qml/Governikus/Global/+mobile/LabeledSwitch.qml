/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	property alias title: titleText.text
	property alias description: descriptionText.text
	property alias checked: entrySwitch.checked

	property real contentMarginVertical: Constants.component_spacing
	property real contentMarginLeft: Constants.component_spacing
	property real contentMarginRight: Constants.component_spacing

	height: implicitHeight
	implicitHeight: Math.max(textContainer.height, entrySwitch.height) + contentMarginVertical

	color: mouseArea.pressed ? Style.color.background_item_pressed : Style.color.transparent

	Accessible.role: Accessible.CheckBox
	Accessible.checkable: enabled
	Accessible.checked: checked
	Accessible.name: title + ". " + description
	Accessible.onToggleAction: entrySwitch.toggle()
	Accessible.onPressAction: entrySwitch.toggle()

	Item {
		id: textContainer

		height: titleText.height + descriptionText.height
		anchors.left: parent.left
		anchors.right: entrySwitch.left
		anchors.rightMargin: Constants.component_spacing * 2
		anchors.leftMargin: contentMarginLeft
		anchors.verticalCenter: parent.verticalCenter

		GText {
			id: titleText

			anchors.left: parent.left
			anchors.right: parent.right

			Accessible.ignored: true

			textStyle: Style.text.normal_accent
		}

		GText {
			id: descriptionText

			anchors.left: parent.left
			anchors.right: parent.right
			anchors.top: titleText.bottom
			anchors.topMargin: 2

			Accessible.ignored: true

			textStyle: Style.text.normal_secondary
		}
	}

	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: entrySwitch.toggle()
	}

	GSwitch {
		id: entrySwitch

		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		anchors.rightMargin: contentMarginRight

		Accessible.ignored: true

		text: titleText.text
	}
}
