/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	id: baseItem

	property real contentMarginLeft: Constants.component_spacing
	property real contentMarginRight: Constants.component_spacing
	property real contentMarginVertical: Constants.component_spacing
	property alias description: descriptionText.text
	property var icon: "qrc:///images/mobile/material_arrow_right.svg"
	property alias tintIcon: iconItem.tintEnabled
	property alias title: titleText.text

	signal clicked

	Accessible.name: title + ". " + description
	Accessible.role: Accessible.Button
	color: mouseArea.pressed ? Style.color.background_item_pressed : Style.color.transparent
	height: implicitHeight
	implicitHeight: Math.max(textContainer.height, iconItem.height) + contentMarginVertical

	Accessible.onPressAction: clicked()

	Item {
		id: textContainer
		anchors.left: parent.left
		anchors.leftMargin: contentMarginLeft
		anchors.right: iconItem.left
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
	TintableIcon {
		id: iconItem
		Accessible.ignored: true
		anchors.right: parent.right
		anchors.rightMargin: contentMarginRight
		anchors.verticalCenter: parent.verticalCenter
		source: baseItem.icon
		sourceSize.height: Style.dimens.small_icon_size
		tintColor: Style.color.secondary_text
		width: Style.dimens.icon_size
	}
	MouseArea {
		id: mouseArea
		anchors.fill: parent

		onClicked: baseItem.clicked()
	}
}
