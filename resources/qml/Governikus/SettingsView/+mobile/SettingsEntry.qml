/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.0
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property alias title: titleText.text
	property alias description: descriptionText.text
	property var icon: "qrc:///images/arrowRight.svg"
	property alias tintIcon: iconItem.tintEnabled
	signal clicked()

	height: Math.max(textContainer.height, iconItem.height)

	Item {
		id: textContainer

		height: titleText.height + descriptionText.height
		anchors.left: parent.left
		anchors.right: iconItem.left
		anchors.rightMargin: Constants.component_spacing * 2
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

	TintableIcon {
		id: iconItem

		width: Style.dimens.icon_size
		sourceSize.height: Style.dimens.small_icon_size
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter

		Accessible.role: Accessible.Button
		Accessible.name: description

		source: baseItem.icon
		tintColor: Style.color.secondary_text
	}

	MouseArea {
		anchors.fill: parent

		onClicked: baseItem.clicked()
	}
}
