import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Type.ApplicationModel 1.0


CheckBox {
	id: control

	indicator: Rectangle {
		anchors.fill: parent
		color: Constants.blue
		radius: Math.max(ApplicationModel.scaleFactor * 4, 1)

		Image {
			source: "qrc:///images/check.svg"
			anchors.fill: parent
			anchors.margins: Math.max(ApplicationModel.scaleFactor * 4, 1)
			fillMode: Image.PreserveAspectFit
			visible: control.checked
		}
	}
}
