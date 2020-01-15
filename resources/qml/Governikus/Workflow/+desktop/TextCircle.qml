/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0


Rectangle {
	property alias text: number.text

	height: number.height * 2
	width: height
	enabled: false
	radius: width * 0.5
	color: enabled ? Style.color.accent : Style.color.background_pane
	border.width: Style.dimens.high_contrast_item_border
	border.color: Style.color.high_contrast_item_border

	GText {
		id: number
		anchors.centerIn: parent
		font.bold: true
		textStyle: parent.enabled ? Style.text.header_inverse : Style.text.header_accent
	}
}
