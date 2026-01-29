/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global

RowLayout {
	property alias text: content.text

	GText {
		Accessible.ignored: true
		Layout.alignment: Qt.AlignTop
		text: " •"
	}
	GText {
		id: content

	}
}
