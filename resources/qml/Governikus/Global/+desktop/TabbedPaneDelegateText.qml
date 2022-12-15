/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Style 1.0

GText {
	id: root

	property string sectionName

	elide: Text.ElideRight
	maximumLineCount: 2
	text: sectionName
	textStyle: isCurrentItem ? Style.text.header : Style.text.header_inverse
	width: parent.width
}
