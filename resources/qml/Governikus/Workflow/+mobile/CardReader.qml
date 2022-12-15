/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: baseItem

	property bool cardAnimation: true
	property bool pinFieldAnimation: true

	width: height * 3 / 7

	Timer {
		id: onTimer

		property int index

		interval: 1500
		running: pinFieldAnimation || cardAnimation
		triggeredOnStart: true

		onTriggered: {
			onTimer.triggeredOnStart = false;
			if (pinFieldAnimation) {
				onTimer.index = ApplicationModel.randomInt(0, 8);
				repeater.itemAt(onTimer.index).state = "on";
			}
			if (cardAnimation) {
				card.state = "out";
			}
			offTimer.restart();
		}
	}
	Timer {
		id: offTimer
		interval: 500

		onTriggered: {
			if (onTimer.index != undefined) {
				repeater.itemAt(onTimer.index).state = "off";
			}
			card.state = "in";
			onTimer.restart();
		}
	}
	Rectangle {
		id: reader
		anchors.bottom: parent.bottom
		color: Style.color.card_reader
		height: parent.height * 5 / 7
		radius: height * 0.05
		width: parent.width

		Rectangle {
			id: slot
			anchors.left: parent.left
			anchors.leftMargin: parent.width * 0.2
			anchors.right: parent.right
			anchors.rightMargin: parent.width * 0.2
			anchors.top: parent.top
			anchors.topMargin: parent.height * 0.05
			color: Constants.white
			height: reader.height * 0.05
			radius: 10
		}
		Rectangle {
			id: card
			anchors.horizontalCenter: parent.horizontalCenter
			color: Style.color.accent
			height: baseItem.height * 1.5 / 7
			radius: height * 0.05
			width: baseItem.width * 0.5

			states: [
				State {
					name: "out"

					AnchorChanges {
						anchors.bottom: reader.top
						target: card
					}
				},
				State {
					name: "in"

					AnchorChanges {
						anchors.bottom: slot.verticalCenter
						target: card
					}
				}
			]
			transitions: [
				Transition {
					AnchorAnimation {
						duration: 500
					}
				}
			]

			onVisibleChanged: y = reader.mapFromItem(baseItem, baseItem.x, card.y).y

			Rectangle {
				id: cardStripe1
				anchors.left: parent.left
				anchors.leftMargin: parent.width * 0.1
				anchors.right: parent.right
				anchors.rightMargin: parent.width * 0.1
				anchors.top: parent.top
				anchors.topMargin: parent.height * 0.1
				color: Constants.white
				height: parent.height * 0.1
				radius: 10
			}
			Rectangle {
				id: cardStripe2
				anchors.left: parent.left
				anchors.leftMargin: parent.width * 0.1
				anchors.right: parent.right
				anchors.rightMargin: parent.width * 0.1
				anchors.top: cardStripe1.top
				anchors.topMargin: parent.height * 0.2
				color: Constants.white
				height: parent.height * 0.1
				radius: cardStripe1.radius
			}
			MouseArea {
				anchors.fill: parent

				onClicked: card.state = card.state === "out" ? "in" : "out"
			}
		}
		Rectangle {
			id: display

			readonly property int margin: parent.width * 0.1

			anchors.bottom: pinGrid.top
			anchors.left: parent.left
			anchors.margins: margin
			anchors.right: parent.right
			color: Constants.white
			height: reader.height * 0.2
			radius: height * 0.2
		}
		Grid {
			id: pinGrid
			anchors.bottom: reader.bottom
			anchors.left: parent.left
			anchors.margins: display.margin
			anchors.right: parent.right
			columns: 3
			height: width
			spacing: anchors.margins

			Repeater {
				id: repeater
				model: 9

				Item {
					id: pinButton

					readonly property int _size: (reader.width - 4 * pinGrid.spacing) / 3
					property alias state: pinButtonCircle.state

					height: width
					width: _size

					Rectangle {
						id: pinButtonCircle
						anchors.centerIn: parent
						height: width
						radius: width * 0.5
						state: "off"
						width: pinButton._size

						Behavior on color  {
							ColorAnimation {
								duration: 250
							}
						}
						states: [
							State {
								name: "off"

								PropertyChanges {
									color: Constants.white
									target: pinButtonCircle
								}
								PropertyChanges {
									target: pinButtonCircle
									width: pinButton._size
								}
							},
							State {
								name: "on"

								PropertyChanges {
									color: Constants.blue
									target: pinButtonCircle
								}
								PropertyChanges {
									target: pinButtonCircle
									width: pinButton._size + pinButton._size * 0.3
								}
							}
						]
						Behavior on width  {
							NumberAnimation {
								duration: 1000
								easing.type: Easing.OutElastic
							}
						}

						MouseArea {
							anchors.fill: parent

							onClicked: pinButton.state = pinButton.state === "off" ? "on" : "off"
						}
					}
				}
			}
		}
	}
}
