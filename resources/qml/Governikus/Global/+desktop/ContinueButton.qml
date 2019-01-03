import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.View 1.0


Button {
	id: control
	width: height

	Accessible.role: Accessible.Button
	Accessible.name: qsTr("Continue")

	FocusFrame {}

	background: Rectangle {
		anchors.fill: parent
		anchors.margins: control.down ? control.height / 32 : 0
		radius: height / 2
		border.width: height / 40;
		border.color: Constants.white
		color: "transparent"

		Rectangle {
			radius: height / 2
			anchors.fill: parent
			anchors.margins: parent.height / 8;
			color: Qt.lighter(Constants.blue, 1.1)

			Image {
				anchors.centerIn: parent
				sourceSize.height: parent.height / 2;
				source: "qrc:///images/desktop/continue_arrow.svg"
			}
		}
	}
}
