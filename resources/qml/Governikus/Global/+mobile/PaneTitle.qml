/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Style 1.0

GText {
	visible: text !== ""
	anchors.left: parent.left
	anchors.right: parent.right

	Accessible.role: Accessible.TitleBar
	Accessible.name: text

	textStyle: Style.text.header_accent
	elide: Text.ElideRight
}
