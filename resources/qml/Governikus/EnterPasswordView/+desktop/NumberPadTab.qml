/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Type.ApplicationModel 1.0


Rectangle {
	width: height / 2 + radius

	border.color: focus ? Constants.white : Constants.blue
	border.width: Math.max(1, ApplicationModel.scaleFactor * 3)
	color: Qt.lighter(Constants.blue, 1.15)
	radius: height / 5

	Timer {
		id: onTimer

		property int index

		interval: 1500
		running: true
		triggeredOnStart: true

		onTriggered: {
			onTimer.triggeredOnStart = false
			onTimer.index = Utils.getRandomInt(0, 12)
			repeater.itemAt(onTimer.index).state = "on"
			offTimer.restart()
		}
	}

	Timer {
		id: offTimer

		interval: 500
		onTriggered: {
			if (onTimer.index != undefined)
			{
				repeater.itemAt(onTimer.index).state = ""
			}

			onTimer.restart()
		}
	}

	Rectangle {
		id: reader

		width: 0.75 * height
		height: parent.height - 2 * parent.radius

		anchors.left: parent.left
		anchors.leftMargin: Math.max(1, ApplicationModel.scaleFactor * 4)
		anchors.verticalCenter: parent.verticalCenter

		color: "skyblue"
		radius: height * 0.1

		Grid {
			id: pinGrid

			anchors.fill: parent

			columns: 3
			padding: parent.width * 0.1
			spacing: parent.width * 0.1

			Repeater {
				id: repeater

				model: 12

				Item {
					id: pinButton

					width: (reader.width - 4 * pinGrid.spacing) / 3
					height: width

					Rectangle {
						id: pinButtonCircle

						width: pinButton.width
						height: width

						anchors.centerIn: parent

						state: parent.state
						radius: width * 0.5
						color: index == 9 ? "yellow" : index == 11 ? "lime" : Constants.white

						Behavior on color {
							ColorAnimation {
								duration: 250
							}
						}

						Behavior on width {
							NumberAnimation {
								duration: 1000
								easing.type: Easing.OutElastic
							}
						}

						states: [
							State {
								name: "on"

								PropertyChanges {target: pinButtonCircle; color: Constants.blue}
								PropertyChanges {target: pinButtonCircle; width: pinButton.width + pinButton.width * 0.3}
							}
						]
					}
				}
			}
		}
	}
}
