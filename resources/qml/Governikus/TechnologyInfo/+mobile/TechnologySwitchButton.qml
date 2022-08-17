/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

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
	Accessible.onPressAction: clicked(null)

	TintableIcon {
		id: img

		anchors.top: parent.top
		anchors.topMargin: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter

		tintEnabled: !parent.enabled || buttonActive
		desaturate: true
		sourceSize.height: Style.dimens.icon_size
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
}
