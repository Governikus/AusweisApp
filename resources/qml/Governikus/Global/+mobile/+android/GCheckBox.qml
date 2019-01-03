import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

import "Utils.js" as Utils


CheckBox {
	id: control

	indicator: Rectangle {
		implicitHeight: Utils.dp(20)
		implicitWidth: Utils.dp(20)
		anchors.centerIn: parent

		color: control.checked ? Constants.accent_color : Constants.white
		border.color: control.checked ? Constants.accent_color : Constants.black
		border.width: Utils.dp(2)
		radius: Utils.dp(2)

		Image {
			source: "qrc:///images/check.svg"
			anchors.fill: parent
			anchors.margins: Utils.dp(3)
			fillMode: Image.PreserveAspectFit
			visible: control.checked
		}
	}
}
