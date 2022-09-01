/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0

Item {
	id: root

	property int passwordType: PasswordType.PIN
	property real scaleFactorGeneral: 1
	property real scaleFactorCan: 1
	property alias textStyle: imageDescriptionText.textStyle

	visible: infoImage.source.toString() !== ""

	implicitHeight: infoImage.implicitHeight + (imageDescriptionText.visible ? (Constants.component_spacing + imageDescriptionText.implicitHeight) : 0)
	implicitWidth: infoImageContainer.width
	height: root.implicitHeight
	width: root.implicitWidth

	Timer {
		id: imageChangeTimer

		property bool alternativeLetter: true

		interval: 6000
		running: passwordType === PasswordType.PIN || passwordType === PasswordType.PUK
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
			sourceSize.width: passwordType === PasswordType.CAN ? 400 * scaleFactorCan : 200 * scaleFactorGeneral
			anchors.fill: parent

			fillMode: Image.PreserveAspectFit
			source: passwordType === PasswordType.CAN ? "qrc:///images/id_card.png"
					: passwordType === PasswordType.REMOTE_PIN ? "qrc:///images/phone_to_pc.svg"
					: passwordType === PasswordType.SMART_PIN || passwordType === PasswordType.NEW_SMART_PIN || passwordType === PasswordType.SMART_BLOCKING_CODE ? ""
					: "qrc:///images/pin_letter_pinpuk_same_page.svg"

			Rectangle {
				readonly property real leftEdge: (parent.width - parent.paintedWidth) / 2.0
				readonly property real topEdge: (parent.height - parent.paintedHeight) / 2.0

				visible: passwordType === PasswordType.CAN
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

			visible: passwordType === PasswordType.PIN || passwordType === PasswordType.PUK

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

		visible: passwordType === PasswordType.PIN || passwordType === PasswordType.PUK
		width: root.width

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom

		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.normal
		text: (imageChangeTimer.alternativeLetter ?
			//: LABEL
			qsTr("ID card PIN/PUK on different pages") :
			//: LABEL
			qsTr("ID card PIN/PUK on the same page")
			)
		wrapMode: Text.Wrap
	}
}
