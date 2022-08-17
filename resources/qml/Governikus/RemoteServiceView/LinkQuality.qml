/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Style 1.0

Image {
	id: img

	property var percent
	property bool inactive: false

	fillMode: Image.PreserveAspectFit
	sourceSize.width: Style.dimens.icon_size
	source: {
		if (inactive) {
			return "qrc:///images/icon_remote_inactive.svg"
		}
		if (percent >= 80) {
			return "qrc:///images/icon_remote_100.svg"
		}
		if (percent >= 60) {
			return "qrc:///images/icon_remote_75.svg"
		}
		if (percent >= 40) {
			return "qrc:///images/icon_remote_50.svg"
		}
		if (percent >= 20) {
			return "qrc:///images/icon_remote_25.svg"
		}
		return "qrc:///images/icon_remote_0.svg"
	}
}
