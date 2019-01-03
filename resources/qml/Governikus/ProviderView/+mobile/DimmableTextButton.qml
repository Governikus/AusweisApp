import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

Label {
	id: label
	signal clicked

	color: white

	MouseArea {
		id: buttonArea
		anchors.fill: parent
		onClicked: {
			parent.clicked()
		}
	}

	states: [
		State {
			name: "pressed"
			when: buttonArea.pressed

			PropertyChanges {
				target: label
				color: Constants.grey
			}
		}
	]
	transitions: [
	]
}
