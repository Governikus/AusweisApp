/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Global 1.0
import Governikus.Style 1.0

MouseArea {
	id: root

	property alias imageSource: img.source
	property alias text: infoText.text
	property bool buttonActive

	height: img.height + infoText.height + 2 * img.anchors.topMargin + infoText.anchors.topMargin
	width: img.width + 2 * 10

	Accessible.role: Accessible.Button
	Accessible.name: text
	Accessible.onPressAction: if (Qt.platform.os === "ios") clicked(null)

	GSeparator {
		width: parent.width * 1.5
		anchors.horizontalCenter: parent.horizontalCenter
	}

	Image {
		id: img
		anchors.top: parent.top
		anchors.topMargin: 20
		anchors.horizontalCenter: parent.horizontalCenter
		sourceSize.height: 50
		fillMode: Image.PreserveAspectFit
		smooth: true
	}

	GText {
		id: infoText

		anchors.top: img.bottom
		anchors.topMargin: 10
		anchors.horizontalCenter: img.horizontalCenter

		Accessible.ignored: true

		textStyle: (!parent.enabled || buttonActive) ? Style.text.normal_secondary : Style.text.normal_accent
	}

	Colorize {
		id: grayLevel
		source: img
		anchors.fill: img
		saturation: 0
		hue: 0
		lightness: 0.0
		cached: true
		visible: !parent.enabled || buttonActive
	}
}
