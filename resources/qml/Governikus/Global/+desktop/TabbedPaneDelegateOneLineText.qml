/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Style 1.0

GText {
	id: root

	property string sectionName

	width: parent.width

	textStyle: Style.text.header
	text: sectionName
	maximumLineCount: 1
	elide: Text.ElideRight
}
