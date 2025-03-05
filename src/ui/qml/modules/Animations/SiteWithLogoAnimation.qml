/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Style

TintableIcon {
	property bool animated: false

	source: "qrc:///animations/siteWithLogo.svg"
	tintColor: Style.color.image

	Image {
		source: "qrc:///images/npa.svg"
		sourceSize.height: parent.height * 0.395

		anchors {
			horizontalCenter: parent.left
			horizontalCenterOffset: parent.width * 0.723
			verticalCenter: parent.top
			verticalCenterOffset: parent.height * 0.477
		}
	}
}
