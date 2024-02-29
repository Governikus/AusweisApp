/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style

Item {
	property alias text: t.text

	height: 50
	state: "inactive"
	width: height

	states: [
		State {
			name: "active"
		},
		State {
			name: "inactive"
		}
	]
	transitions: [
		Transition {
			from: "inactive"
			to: "active"

			SequentialAnimation {
				ParallelAnimation {
					NumberAnimation {
						duration: 1000
						easing.type: Easing.OutElastic
						property: "font.pixelSize"
						target: t
					}
					ColorAnimation {
						duration: 100
					}
				}
				PropertyAnimation {
					property: "running"
					target: busy
					to: true
				}
			}
		},
		Transition {
			from: "active"
			to: "inactive"

			SequentialAnimation {
				PropertyAction {
					property: "running"
					target: busy
					value: false
				}
				NumberAnimation {
					duration: 200
					property: "font.pixelSize"
					target: t
				}
			}
		}
	]

	GBusyIndicator {
		id: busy

		anchors.centerIn: parent
		height: rec.height * 1.1
		running: false
		width: height
	}
	Rectangle {
		id: rec

		anchors.centerIn: parent
		border.color: Constants.blue
		border.width: 1
		color: parent.state === "active" ? Style.color.control : Constants.white
		height: parent.state === "active" ? parent.height : parent.height / 2
		radius: width * 0.5
		width: height
	}
	GText {
		id: t

		anchors.centerIn: rec
		color: parent.state === "active" ? Constants.white : Style.color.control
		font.bold: parent.state === "active"
	}
}
