/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	property alias source: tabImage.source
	property alias text: tabText.text
	property var selected: false
	property var iconPadding: 4
	signal clicked

	Accessible.role: Accessible.PageTab
	Accessible.name: text
	Accessible.onPressAction: clicked()

	Image {
		id: tabImage
		anchors.top: parent.top
		anchors.bottom: tabText.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: iconPadding

		fillMode: Image.PreserveAspectFit
	}

	GText {
		id: tabText

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom

		Accessible.ignored: true

		textStyle: Style.text.hint
	}

	MouseArea {
		anchors.fill: parent
		onClicked: parent.clicked()
	}

	layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
	layer.effect: ColorOverlay {
		color: selected ? Style.text.hint_accent.textColor : Style.text.hint_secondary.textColor
	}
}
