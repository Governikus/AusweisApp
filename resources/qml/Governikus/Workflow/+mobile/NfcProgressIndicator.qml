/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.CardPositionModel 1.0

Item {
	id: root

	property real animateInDuration: 0.3 * cardPositionModel.cyclingClock
	property real animateOutDuration: 0.2 * cardPositionModel.cyclingClock
	property var cardPosition: null
	property bool startPositionLeft: true

	states: [
		State {
			name: "off"

			PropertyChanges {
				target: phone
				tintEnabled: true
			}
			PropertyChanges {
				opacity: 0
				restoreEntryValues: false
				target: card
			}
			PropertyChanges {
				running: false
				target: modelActivationTimer
			}
			PropertyChanges {
				restoreEntryValues: false
				running: false
				target: cardPositionModel
			}
		},
		State {
			name: "on"

			PropertyChanges {
				target: phone
				tintEnabled: false
			}
			PropertyChanges {
				running: true
				target: modelActivationTimer
			}
		}
	]

	Connections {
		function onFireCardPositionChanged() {
			cardPosition = cardPositionModel.getCardPosition();
			animation.restart();
		}

		target: cardPositionModel
	}
	CardPositionModel {
		id: cardPositionModel
	}
	Timer {
		id: modelActivationTimer
		interval: 1000

		onTriggered: cardPositionModel.running = true
	}
	SequentialAnimation {
		id: animation

		readonly property int pauseDuration: Math.max(cardPositionModel.cyclingClock - root.animateInDuration - root.animateOutDuration, 0)

		alwaysRunToEnd: false
		running: false

		PropertyAction {
			property: "anchors.horizontalCenterOffset"
			target: card
			value: (startPositionLeft ? -phone.width : phone.width) * 0.75
		}
		PropertyAction {
			property: "anchors.verticalCenterOffset"
			target: card
			value: -phone.height * 0.5
		}
		PropertyAction {
			property: "rotation"
			target: card
			value: 0
		}
		PropertyAction {
			property: "z"
			target: card
			value: cardPosition ? cardPosition.z : 0
		}
		ParallelAnimation {
			NumberAnimation {
				duration: root.animateInDuration
				easing.type: Easing.OutCubic
				property: "opacity"
				target: card
				to: 1
			}
			NumberAnimation {
				duration: root.animateInDuration
				easing.type: Easing.OutCubic
				property: "rotation"
				target: card
				to: cardPosition ? cardPosition.rotation : 0
			}
			NumberAnimation {
				duration: root.animateInDuration
				easing.type: Easing.OutCubic
				property: "anchors.horizontalCenterOffset"
				target: card
				to: phone.width * ((cardPosition ? cardPosition.x : 0) - 0.5)
			}
			NumberAnimation {
				duration: root.animateInDuration
				easing.type: Easing.OutCubic
				property: "anchors.verticalCenterOffset"
				target: card
				to: phone.height * ((cardPosition ? cardPosition.y : 0) - 0.5)
			}
		}
		PauseAnimation {
			duration: animation.pauseDuration * 0.9
		}
		NumberAnimation {
			duration: root.animateOutDuration
			easing.type: Easing.InCubic
			property: "opacity"
			target: card
			to: 0
		}
		PauseAnimation {
			duration: animation.pauseDuration * 0.1
		}
	}
	Image {
		id: card
		anchors.centerIn: phone
		asynchronous: true
		fillMode: Image.PreserveAspectFit
		opacity: 0
		source: "qrc:///images/ausweis.svg"
		sourceSize.height: phone.height * 0.5
		transformOrigin: Item.Center
		visible: !phone.tintEnabled
	}
	TintableIcon {
		id: phone
		anchors.centerIn: parent
		clip: true
		desaturate: true
		height: Math.ceil(parent.height * 0.25) * 2
		opacity: tintEnabled ? 0.7 : 1.0
		source: "qrc:///images/mobile/phone_nfc.svg"
		z: 0

		Image {
			asynchronous: true
			fillMode: Image.PreserveAspectFit
			opacity: card.z < 0 ? card.opacity : 0
			rotation: card.rotation
			source: "qrc:///images/ausweis_outline.svg"
			sourceSize.height: card.sourceSize.height
			transformOrigin: Item.Center
			visible: !phone.tintEnabled
			x: card.x - phone.x
			y: card.y - phone.y
			z: 1
		}
	}
}
