/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Style 1.0

GText {
	id: root

	property string sectionName

	Accessible.role: Accessible.PageTab
	Accessible.name: sectionName

	width: parent.width

	textStyle: Style.text.header_inverse
	text: sectionName
	maximumLineCount: 1
	elide: Text.ElideRight
}
