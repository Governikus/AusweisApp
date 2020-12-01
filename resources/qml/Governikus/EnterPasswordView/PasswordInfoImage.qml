/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.NumberModel 1.0

Item {
	id: root

	property int passwordType: NumberModel.PASSWORD_PIN
	property real scaleFactorGeneral: 1
	property real scaleFactorCan: 1
	property alias textStyle: imageDescriptionText.textStyle

	implicitHeight: infoImage.implicitHeight + Constants.component_spacing + imageDescriptionText.implicitHeight
	implicitWidth: Math.max(infoImageContainer.width, imageDescriptionText.implicitWidth)
	height: root.implicitHeight
	width: root.implicitWidth

	Timer {
		id: imageChangeTimer

		property bool alternativeLetter: true

		interval: 6000
		running: passwordType === NumberModel.PASSWORD_PIN || passwordType === NumberModel.PASSWORD_PUK
		repeat: true
		onTriggered: alternativeLetter = !alternativeLetter
	}

	Item {
		id: infoImageContainer

		width: infoImage.implicitWidth
		height: infoImage.implicitHeight
		anchors.top: parent.top
		anchors.horizontalCenter: parent.horizontalCenter

		Image {
			id: infoImage

			width: parent.width
			sourceSize.width: passwordType === NumberModel.PASSWORD_CAN ? 400 * scaleFactorCan : 200 * scaleFactorGeneral
			anchors.fill: parent

			fillMode: Image.PreserveAspectFit
			source: passwordType === NumberModel.PASSWORD_CAN ? "qrc:///images/id_card.png"
					: passwordType === NumberModel.PASSWORD_REMOTE_PIN ? "qrc:///images/phone_to_pc.svg"
					: "qrc:///images/pin_letter_pinpuk_same_page.svg"

			Rectangle {
				readonly property real leftEdge: (parent.width - parent.paintedWidth) / 2.0
				readonly property real topEdge: (parent.height - parent.paintedHeight) / 2.0

				visible: passwordType === NumberModel.PASSWORD_CAN
				height: parent.paintedWidth * 0.0625
				width: parent.paintedWidth * 0.2
				anchors.top: parent.top
				anchors.topMargin: parent.paintedWidth * 0.4175 + topEdge
				anchors.left: parent.left
				anchors.leftMargin: parent.paintedWidth * 0.7475 + leftEdge

				color: Style.color.transparent
				border.color: Constants.red
				border.width: Math.max(1, parent.paintedWidth * 0.015)
			}
		}

		states: State {
			name: "alternativePinLetter"
			when: imageChangeTimer.alternativeLetter

			PropertyChanges {
				target: pinPukVariantB
				opacity: 1
			}
		}

		transitions: Transition {
			NumberAnimation {
				properties: "opacity"
				easing.type: Easing.InOutQuad
				duration: Constants.animation_duration
			}
		}

		Image {
			id: pinPukVariantB

			visible: passwordType === NumberModel.PASSWORD_PIN || passwordType === NumberModel.PASSWORD_PUK

			opacity: 0.0
			width: parent.width
			sourceSize.width: 200 * scaleFactorGeneral
			anchors.fill: parent

			fillMode: Image.PreserveAspectFit

			source: "qrc:///images/pin_letter_pinpuk_different_page.svg"
		}
	}

	GText {
		id: imageDescriptionText

		visible: passwordType === NumberModel.PASSWORD_PIN || passwordType === NumberModel.PASSWORD_PUK

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom

		textStyle: Style.text.normal
		text: (imageChangeTimer.alternativeLetter ?
			//: LABEL
			qsTr("PIN/PUK on different pages") :
			//: LABEL
			qsTr("PIN/PUK on the same page")
			)
	}
}