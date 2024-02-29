/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.Type.CardPositionModel
import Governikus.Type.SettingsModel

Item {
	id: root

	property real animateInDuration: 0.3 * cardPositionModel.cyclingClock
	property real animateOutDuration: 0.2 * cardPositionModel.cyclingClock
	property var cardPosition: null
	property bool startPositionLeft: true

	implicitHeight: Style.dimens.workflow_progress_indicator_size
	implicitWidth: phone.implicitWidth

	states: [
		State {
			name: "unavailable"

			PropertyChanges {
				modelActivationTimer.running: false
				symbol.source: "qrc:///images/mobile/x.svg"
				symbol.tintColor: Style.color.warning
				symbol.visible: true
			}
			PropertyChanges {
				card.opacity: 0
				cardPositionModel.running: false
				restoreEntryValues: false
			}
		},
		State {
			name: "off"

			PropertyChanges {
				modelActivationTimer.running: false
				symbol.source: "qrc:///images/mobile/questionmark.svg"
				symbol.tintColor: Style.color.image
				symbol.visible: true
			}
			PropertyChanges {
				card.opacity: 0
				cardPositionModel.running: false
				restoreEntryValues: false
			}
		},
		State {
			name: "on"

			PropertyChanges {
				modelActivationTimer.running: SettingsModel.useAnimations
				symbol.visible: false
			}
		}
	]

	Component.onCompleted: {
		if (!SettingsModel.useAnimations) {
			cardPosition = cardPositionModel.getCardPosition();
			animation.start();
			animation.complete();
		}
	}

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
			value: (startPositionLeft ? -fakephone.width : fakephone.width) * 0.75
		}
		PropertyAction {
			property: "anchors.verticalCenterOffset"
			target: card
			value: -fakephone.height * 0.5
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
				to: fakephone.width * ((cardPosition ? cardPosition.x : 0) - 0.5)
			}
			NumberAnimation {
				duration: root.animateInDuration
				easing.type: Easing.OutCubic
				property: "anchors.verticalCenterOffset"
				target: card
				to: fakephone.height * ((cardPosition ? cardPosition.y : 0) - 0.5)
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
			to: SettingsModel.useAnimations ? 0 : 1
		}
		PauseAnimation {
			duration: animation.pauseDuration * 0.1
		}
	}
	TintableIcon {
		id: phone

		anchors.centerIn: parent
		source: symbol.visible ? "qrc:///images/mobile/phone_nfc_info.svg" : "qrc:///images/mobile/phone_nfc.svg"
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.image
		z: 0
	}
	TintableIcon {
		id: card

		anchors.centerIn: fakephone
		asynchronous: true
		fillMode: Image.PreserveAspectFit
		opacity: 0
		source: "qrc:///images/mobile/card.svg"
		sourceSize.height: fakephone.height * 0.5
		tintColor: Style.color.image
		visible: !symbol.visible
	}
	Item {
		id: fakephone

		anchors.bottom: phone.bottom
		anchors.horizontalCenter: phone.horizontalCenter
		anchors.horizontalCenterOffset: -phone.width * 0.06
		anchors.top: phone.top
		clip: true
		width: phone.width * 0.43

		TintableIcon {
			asynchronous: true
			fillMode: Image.PreserveAspectFit
			opacity: card.z < 0 ? card.opacity : 0
			rotation: card.rotation
			source: "qrc:///images/mobile/card.svg"
			sourceSize.height: card.sourceSize.height
			tintColor: Style.color.control_disabled
			visible: card.visible
			x: card.x - fakephone.x
			y: card.y - fakephone.y
			z: 1
		}
	}
	TintableIcon {
		id: symbol

		anchors.horizontalCenter: phone.right
		anchors.horizontalCenterOffset: -phone.width * 0.19
		anchors.verticalCenter: phone.bottom
		anchors.verticalCenterOffset: -phone.height * 0.36
		asynchronous: true
		fillMode: Image.PreserveAspectFit
		sourceSize.height: phone.height * 0.2
	}
}
