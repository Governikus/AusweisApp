/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0

Column {
	width: parent.width

	property alias source: image.source

	TutorialImage {
		id: image
		width: parent.width * 0.03

		centerX: 0.5
		centerY: 0.5
	}
}
