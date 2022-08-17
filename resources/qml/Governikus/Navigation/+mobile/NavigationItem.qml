/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	property alias source: tabImage.source
	property alias text: tabText.text
	property var selected: false
	signal clicked()

	readonly property color itemColor: selected ? Style.text.hint_accent.textColor : Style.text.hint_secondary.textColor

	Accessible.role: Accessible.Button
	Accessible.name: text
	Accessible.onPressAction: clicked()

	TintableIcon {
		id: tabImage

		anchors {
			top: parent.top
			bottom: tabText.top
			left: parent.left
			right: parent.right
			bottomMargin: Style.dimens.navigation_bar_text_padding
		}

		sourceSize.height: Style.dimens.navigation_bar_height
		tintColor: parent.itemColor
	}

	GText {
		id: tabText

		width: parent.width
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom

		Accessible.ignored: true

		maximumLineCount: 1
		horizontalAlignment: Text.AlignHCenter
		elide: Text.ElideRight
		textStyle: Style.text.navigation
		color: parent.itemColor
	}

	MouseArea {
		anchors.fill: parent
		onClicked: parent.clicked()
	}
}
