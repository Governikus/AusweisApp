/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0

Column {
	property alias source: image.source

	width: parent.width

	TutorialImage {
		id: image
		centerX: 0.5
		centerY: 0.5
		width: parent.width * 0.03
	}
}
