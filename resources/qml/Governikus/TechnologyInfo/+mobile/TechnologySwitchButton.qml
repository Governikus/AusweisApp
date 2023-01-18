/*
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

MouseArea {
	id: root

	property bool buttonActive
	property alias imageSource: img.source
	property alias text: infoText.text

	Accessible.name: text
	Accessible.role: Accessible.Button
	height: img.height + infoText.height + img.anchors.topMargin + infoText.anchors.topMargin + infoText.anchors.bottomMargin
	width: Math.max(img.width, infoText.width)

	Accessible.onPressAction: clicked(null)

	TintableIcon {
		id: img
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.top
		anchors.topMargin: Constants.component_spacing
		desaturate: true
		sourceSize.height: Style.dimens.icon_size
		tintEnabled: !parent.enabled || buttonActive
	}
	GText {
		id: infoText
		Accessible.ignored: true
		anchors.bottomMargin: anchors.topMargin
		anchors.horizontalCenter: img.horizontalCenter
		anchors.top: img.bottom
		anchors.topMargin: Constants.text_spacing
		textStyle: (!parent.enabled || buttonActive) ? Style.text.normal_secondary : Style.text.normal_accent
	}
}
