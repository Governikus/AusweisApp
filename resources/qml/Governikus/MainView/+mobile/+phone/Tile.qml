/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0

GPaneBackground {
	id: root

	property alias title: text.text
	property alias image: image.source

	signal clicked()

	Accessible.role: Accessible.Button
	Accessible.name: title
	Accessible.onPressAction: clicked()

	color: Qt.darker(Style.color.accent, mouseArea.pressed ? Constants.highlightDarkerFactor : 1)

	MouseArea {
		id: mouseArea

		anchors.fill: root

		onClicked: root.clicked()
	}

	TintableIcon {
		id: image

		anchors {
			left: root.left
			verticalCenter: root.verticalCenter
			leftMargin: Constants.component_spacing
		}
		height: root.height - Constants.pane_padding

		Accessible.ignored: true

		sourceSize.height: Style.dimens.large_icon_size
		tintColor: text.color
	}

	GText {
		id: text

		anchors {
			right: root.right
			left: image.right
			top: root.top
			bottom: root.bottom
			leftMargin: Constants.text_spacing
			rightMargin: Constants.text_spacing
		}

		Accessible.ignored: true

		elide: Text.ElideRight
		textStyle: Style.text.title_inverse
		color: Qt.darker(textStyle.textColor, mouseArea.pressed ? Constants.highlightDarkerFactor : 1)
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		font.bold: true
	}
}
