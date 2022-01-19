/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	id: baseItem

	property alias title: titleText.text
	property alias description: descriptionText.text
	property var icon: "qrc:///images/mobile/material_arrow_right.svg"
	property alias tintIcon: iconItem.tintEnabled

	property real contentMarginVertical: Constants.component_spacing
	property real contentMarginLeft: Constants.component_spacing
	property real contentMarginRight: Constants.component_spacing

	signal clicked()

	height: implicitHeight
	implicitHeight: Math.max(textContainer.height, iconItem.height) + contentMarginVertical

	color: mouseArea.pressed ? Style.color.background_item_pressed : Style.color.transparent

	Accessible.role: Accessible.Button
	Accessible.name: title
	Accessible.description: description
	Accessible.onPressAction: if (Qt.platform.os === "ios") clicked()

	Item {
		id: textContainer

		height: titleText.height + descriptionText.height

		anchors.left: parent.left
		anchors.right: iconItem.left
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

	TintableIcon {
		id: iconItem

		width: Style.dimens.icon_size
		sourceSize.height: Style.dimens.small_icon_size

		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		anchors.rightMargin: contentMarginRight

		Accessible.ignored: true

		source: baseItem.icon
		tintColor: Style.color.secondary_text
	}

	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: baseItem.clicked()
	}
}
