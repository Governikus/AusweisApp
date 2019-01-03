import QtQuick 2.10

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
