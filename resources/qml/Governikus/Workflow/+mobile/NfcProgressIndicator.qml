/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.CardPositionModel 1.0


Item {
	id: root

	property var cardPosition: null
	property real animateInDuration: 0.3 * cardPositionModel.cyclingClock
	property real animateOutDuration: 0.2 * cardPositionModel.cyclingClock
	property bool startPositionLeft: true

	Connections {
		target: cardPositionModel

		function onFireCardPositionChanged() {
			cardPosition = cardPositionModel.getCardPosition()
			animation.restart()
		}
	}

	CardPositionModel {
		id: cardPositionModel
	}

	Timer {
		id: modelActivationTimer

		interval: 1000

		onTriggered: cardPositionModel.running = true
	}

	states: [
		State {
			name: "off"
			PropertyChanges { target: phone; tintEnabled: true }
			PropertyChanges { target: card; opacity: 0; restoreEntryValues: false }
			PropertyChanges { target: modelActivationTimer; running: false }
			PropertyChanges { target: cardPositionModel; running: false; restoreEntryValues: false }
		},
		State {
			name: "on"
			PropertyChanges { target: phone; tintEnabled: false }
			PropertyChanges { target: modelActivationTimer; running: true }
		}
	]

	SequentialAnimation {
		id: animation

		readonly property int pauseDuration: Math.max(cardPositionModel.cyclingClock - root.animateInDuration - root.animateOutDuration, 0)

		alwaysRunToEnd: false
		running: false

		PropertyAction {
			target: card
			property: "anchors.horizontalCenterOffset"
			value: (startPositionLeft ? -phone.width : phone.width) * 0.75
		}
		PropertyAction {
			target: card
			property: "anchors.verticalCenterOffset"
			value: - phone.height * 0.5
		}
		PropertyAction {
			target: card
			property: "rotation"
			value: 0
		}
		PropertyAction {
			target: card
			property: "z"
			value: cardPosition ? cardPosition.z : 0
		}

		ParallelAnimation {
			NumberAnimation {
				target: card
				property: "opacity"
				to: 1
				duration: root.animateInDuration
				easing.type: Easing.OutCubic
			}
			NumberAnimation {
				target: card
				property: "rotation"
				to: cardPosition ? cardPosition.rotation : 0
				duration: root.animateInDuration
				easing.type: Easing.OutCubic
			}
			NumberAnimation {
				target: card
				property: "anchors.horizontalCenterOffset"
				to: phone.width * ((cardPosition ? cardPosition.x : 0) - 0.5)
				duration: root.animateInDuration
				easing.type: Easing.OutCubic
			}
			NumberAnimation {
				target: card
				property: "anchors.verticalCenterOffset"
				to: phone.height * ((cardPosition ? cardPosition.y : 0) - 0.5)
				duration: root.animateInDuration
				easing.type: Easing.OutCubic
			}
		}

		PauseAnimation {
			duration: animation.pauseDuration * 0.9
		}

		NumberAnimation {
			target: card
			property: "opacity"
			to: 0
			duration: root.animateOutDuration
			easing.type: Easing.InCubic
		}

		PauseAnimation {
			duration: animation.pauseDuration * 0.1
		}
	}

	Image {
		id: card

		visible: !phone.tintEnabled
		anchors.centerIn: phone
		sourceSize.height: phone.height * 0.5
		transformOrigin: Item.Center

		opacity: 0
		source: "qrc:///images/ausweis.svg"
		fillMode: Image.PreserveAspectFit
		asynchronous: true
	}

	TintableIcon {
		id: phone

		z: 0
		anchors.centerIn: parent
		height: Math.ceil(parent.height * 0.25) * 2

		clip: true
		source: "qrc:///images/mobile/phone_nfc.svg"
		desaturate: true
		opacity: tintEnabled ? 0.7 : 1.0

		Image {
			visible: !phone.tintEnabled
			x: card.x - phone.x
			y: card.y - phone.y
			z: 1
			sourceSize.height: card.sourceSize.height
			transformOrigin: Item.Center

			opacity: card.z < 0 ? card.opacity : 0
			rotation: card.rotation
			source: "qrc:///images/ausweis_outline.svg"
			fillMode: Image.PreserveAspectFit
			asynchronous: true
		}
	}
}
