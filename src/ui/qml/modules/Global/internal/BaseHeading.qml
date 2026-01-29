/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

GText {
	Accessible.name: {
		if (Qt.platform.os !== "windows") {
			return text;
		}
		//: WINDOWS Screenreader announcement, that the current item is a heading.
		return text + " , " + qsTr("Heading");
	}
	Accessible.role: Accessible.Heading
}
