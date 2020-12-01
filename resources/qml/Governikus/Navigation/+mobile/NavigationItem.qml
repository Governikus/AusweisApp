/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtGraphicalEffects 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	property alias source: tabImage.source
	property alias text: tabText.text
	property var selected: false
	signal clicked

	Accessible.role: Accessible.PageTab
	Accessible.name: text
	Accessible.onPressAction: clicked()

	Image {
		id: tabImage

		anchors {
			top: parent.top
			bottom: tabText.top
			left: parent.left
			right: parent.right
			bottomMargin: Style.dimens.navigation_bar_text_padding
		}

		sourceSize.height: height
		fillMode: Image.PreserveAspectFit
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
