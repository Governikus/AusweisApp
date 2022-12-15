/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	readonly property color itemColor: selected ? Style.text.hint_accent.textColor : Style.text.hint_secondary.textColor
	property var selected: false
	property alias source: tabImage.source
	property alias text: tabText.text

	signal clicked

	Accessible.name: text
	Accessible.role: Accessible.Button

	Accessible.onPressAction: clicked()

	TintableIcon {
		id: tabImage
		sourceSize.height: Style.dimens.navigation_bar_height
		tintColor: parent.itemColor

		anchors {
			bottom: tabText.top
			bottomMargin: Style.dimens.navigation_bar_text_padding
			left: parent.left
			right: parent.right
			top: parent.top
		}
	}
	GText {
		id: tabText
		Accessible.ignored: true
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		color: parent.itemColor
		elide: Text.ElideRight
		horizontalAlignment: Text.AlignHCenter
		maximumLineCount: 1
		textStyle: Style.text.navigation
		width: parent.width
	}
	MouseArea {
		anchors.fill: parent

		onClicked: parent.clicked()
	}
}
