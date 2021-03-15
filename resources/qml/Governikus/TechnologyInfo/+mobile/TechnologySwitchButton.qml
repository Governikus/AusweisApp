/*
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtGraphicalEffects 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

MouseArea {
	id: root

	property alias imageSource: img.source
	property alias text: infoText.text
	property bool buttonActive

	height: img.height + infoText.height + img.anchors.topMargin + infoText.anchors.topMargin + infoText.anchors.bottomMargin
	width: Math.max(img.width, infoText.width)

	Accessible.role: Accessible.Button
	Accessible.name: text
	Accessible.onPressAction: if (Qt.platform.os === "ios") clicked(null)

	Image {
		id: img

		anchors.top: parent.top
		anchors.topMargin: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter

		sourceSize.height: Style.dimens.icon_size
		fillMode: Image.PreserveAspectFit
		smooth: true
	}

	GText {
		id: infoText

		anchors.top: img.bottom
		anchors.topMargin: Constants.text_spacing
		anchors.bottomMargin: anchors.topMargin
		anchors.horizontalCenter: img.horizontalCenter

		Accessible.ignored: true

		textStyle: (!parent.enabled || buttonActive) ? Style.text.normal_secondary : Style.text.normal_accent
	}

	Colorize {
		id: grayLevel

		visible: !parent.enabled || buttonActive
		anchors.fill: img

		source: img
		saturation: 0
		hue: 0
		lightness: 0.0
		cached: true
	}
}
