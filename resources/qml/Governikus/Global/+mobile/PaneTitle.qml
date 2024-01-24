/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Style

GText {
	Accessible.name: text
	Accessible.role: Accessible.TitleBar
	elide: Text.ElideRight
	textStyle: Style.text.headline
	visible: text !== ""
}
