import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

import "Utils.js" as Utils


CheckBox {
	id: control

	indicator: Rectangle {
		implicitHeight: Utils.dp(26)
		implicitWidth: Utils.dp(26)
		anchors.centerIn: parent

		color: Constants.white
		border.color: Constants.black
		border.width: Utils.dp(1)

		Image {
			source: "qrc:///images/iOS/CheckedCheckbox.png"
			anchors.fill: parent
			anchors.margins: Utils.dp(2)
			fillMode: Image.PreserveAspectFit
			visible: control.checked
		}
	}
}
