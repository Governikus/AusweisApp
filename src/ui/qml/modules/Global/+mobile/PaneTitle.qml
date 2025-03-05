/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Style

GText {
	Accessible.role: Accessible.TitleBar
	elide: Text.ElideRight
	textStyle: Style.text.headline
	visible: text !== ""
}
