/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Style 1.0

GText {
	visible: text !== ""

	Accessible.role: Accessible.TitleBar
	Accessible.name: text

	textStyle: Style.text.header_accent
	elide: Text.ElideRight
}
