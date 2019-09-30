/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
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
	color: enabled ? Style.color.accent : Constants.white

	GText {
		id: number
		anchors.centerIn: parent
		font.bold: true
		textStyle: parent.enabled ? Style.text.header : Style.text.header_accent
	}
}
