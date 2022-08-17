/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	property bool pinFieldAnimation: true
	property bool cardAnimation: true

	id: baseItem
	width: height * 3 / 7

	Timer {
		property int index

		id: onTimer
		interval: 1500
		running: pinFieldAnimation || cardAnimation
		triggeredOnStart: true

		onTriggered: {
			onTimer.triggeredOnStart = false
			if (pinFieldAnimation)
			{
				onTimer.index = ApplicationModel.randomInt(0, 8)
				repeater.itemAt(onTimer.index).state = "on"
			}

			if (cardAnimation)
			{
				card.state = "out"
			}

			offTimer.restart()
		}
	}

	Timer {
		id: offTimer
		interval: 500
		onTriggered: {
			if (onTimer.index != undefined)
			{
				repeater.itemAt(onTimer.index).state = "off"
			}

			card.state = "in"

			onTimer.restart()
		}
	}



	Rectangle {
		id: reader
		color: Style.color.card_reader
		radius: height * 0.05
		anchors.bottom: parent.bottom
		width: parent.width
		height: parent.height * 5 / 7

		Rectangle {
			id: slot
			color: Constants.white
			radius: 10
			height: reader.height * 0.05
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.topMargin: parent.height * 0.05
			anchors.leftMargin: parent.width * 0.2
			anchors.rightMargin: parent.width * 0.2
		}

		Rectangle {
			id: card
			color: Style.color.accent
			radius: height * 0.05
			anchors.horizontalCenter: parent.horizontalCenter
			height: baseItem.height * 1.5 / 7
			width: baseItem.width * 0.5

			onVisibleChanged: y = reader.mapFromItem(baseItem, baseItem.x, card.y).y

			Rectangle {
				id: cardStripe1
				color: Constants.white
				radius: 10
				height: parent.height * 0.1
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.topMargin: parent.height * 0.1
				anchors.leftMargin: parent.width * 0.1
				anchors.rightMargin: parent.width * 0.1
			}

			Rectangle {
				id: cardStripe2
				color: Constants.white
				radius: cardStripe1.radius
				height: parent.height * 0.1
				anchors.top: cardStripe1.top
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.topMargin: parent.height * 0.2
				anchors.leftMargin: parent.width * 0.1
				anchors.rightMargin: parent.width * 0.1
			}

			MouseArea {
				anchors.fill: parent
				onClicked: card.state = card.state === "out" ? "in" : "out"
			}

			states: [
				State {
					name: "out"
					AnchorChanges {target: card; anchors.bottom: reader.top}
				},
				State {
					name: "in"
					AnchorChanges {target: card; anchors.bottom: slot.verticalCenter}
				}
			]

			transitions: [
				Transition {
					AnchorAnimation {duration: 500}
				}
			]
		}

		Rectangle {
			readonly property int margin: parent.width * 0.1
			id: display
			color: Constants.white
			radius: height * 0.2
			height: reader.height * 0.2
			anchors.bottom: pinGrid.top
			anchors.margins: margin
			anchors.left: parent.left
			anchors.right: parent.right
		}

		Grid {
			id: pinGrid
			anchors.bottom: reader.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.margins: display.margin

			columns: 3
			spacing: anchors.margins
			height: width

			Repeater {
				id: repeater
				model: 9

				Item {
					readonly property int _size: (reader.width - 4 * pinGrid.spacing) / 3
					property alias state: pinButtonCircle.state

					id: pinButton
					width: _size
					height: width

					Rectangle {
						id: pinButtonCircle
						anchors.centerIn: parent
						width: pinButton._size
						height: width
						radius: width * 0.5

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

						MouseArea {
							anchors.fill: parent
							onClicked: pinButton.state = pinButton.state === "off" ? "on" : "off"
						}

						state: "off"

						states: [
							State {
								name: "off"
								PropertyChanges {target: pinButtonCircle; color: Constants.white}
								PropertyChanges {target: pinButtonCircle; width: pinButton._size}
							},
							State {
								name: "on"
								PropertyChanges {target: pinButtonCircle; color: Constants.blue}
								PropertyChanges {target: pinButtonCircle; width: pinButton._size + pinButton._size * 0.3}
							}
						]
					}
				}
			}
		}
	}
}
