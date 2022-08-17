/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Style 1.0

GText {
	id: root

	property string sectionName

	width: parent.width

	textStyle: isCurrentItem ? Style.text.header : Style.text.header_inverse
	text: sectionName
	maximumLineCount: 1
	elide: Text.ElideRight
}
