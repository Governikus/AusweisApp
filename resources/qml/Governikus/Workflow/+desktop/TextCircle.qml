/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	property alias text: number.text

	border.color: Style.color.high_contrast_item_border
	border.width: Style.dimens.high_contrast_item_border
	color: enabled ? Style.color.accent : Style.color.background_pane
	enabled: false
	height: number.height * 2
	radius: width * 0.5
	width: height

	GText {
		id: number
		Accessible.ignored: true
		anchors.centerIn: parent
		textStyle: parent.enabled ? Style.text.header_inverse_highlight : Style.text.header_accent_highlight
	}
}
