/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

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
