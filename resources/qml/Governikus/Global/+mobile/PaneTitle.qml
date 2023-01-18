/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Style 1.0

GText {
	Accessible.name: text
	Accessible.role: Accessible.TitleBar
	elide: Text.ElideRight
	textStyle: Style.text.header_accent
	visible: text !== ""
}
