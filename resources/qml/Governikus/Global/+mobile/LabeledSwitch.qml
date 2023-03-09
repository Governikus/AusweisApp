/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	property alias checked: entrySwitch.checked
	property real contentMarginLeft: Constants.component_spacing
	property real contentMarginRight: Constants.component_spacing
	property real contentMarginVertical: Constants.component_spacing
	property alias description: descriptionText.text
	property alias title: titleText.text

	Accessible.checkable: enabled
	Accessible.checked: checked
	Accessible.name: title + ". " + description
	Accessible.role: Accessible.CheckBox
	color: mouseArea.pressed ? Style.color.background_item_pressed : Style.color.transparent
	height: implicitHeight
	implicitHeight: Math.max(textContainer.height, entrySwitch.height) + contentMarginVertical

	Accessible.onPressAction: entrySwitch.toggle()
	Accessible.onToggleAction: entrySwitch.toggle()

	Item {
		id: textContainer
		anchors.left: parent.left
		anchors.leftMargin: contentMarginLeft
		anchors.right: entrySwitch.left
		anchors.rightMargin: Constants.component_spacing * 2
		anchors.verticalCenter: parent.verticalCenter
		height: titleText.height + descriptionText.height

		GText {
			id: titleText
			Accessible.ignored: true
			anchors.left: parent.left
			anchors.right: parent.right
			textStyle: Style.text.normal_accent
		}
		GText {
			id: descriptionText
			Accessible.ignored: true
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.top: titleText.bottom
			anchors.topMargin: 2
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
		Accessible.ignored: true
		anchors.right: parent.right
		anchors.rightMargin: contentMarginRight
		anchors.verticalCenter: parent.verticalCenter
		text: titleText.text
	}
}
