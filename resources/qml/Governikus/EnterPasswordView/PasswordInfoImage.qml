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
	property real scaleFactorCan: 1
	property real scaleFactorGeneral: 1
	property alias textStyle: imageDescriptionText.textStyle

	height: root.implicitHeight
	implicitHeight: infoImage.implicitHeight + (imageDescriptionText.visible ? (Constants.component_spacing + imageDescriptionText.implicitHeight) : 0)
	implicitWidth: infoImageContainer.width
	visible: infoImage.source.toString() !== ""
	width: root.implicitWidth

	Timer {
		id: imageChangeTimer

		property bool alternativeLetter: true

		interval: 6000
		repeat: true
		running: passwordType === PasswordType.PIN || passwordType === PasswordType.PUK

		onTriggered: alternativeLetter = !alternativeLetter
	}
	Item {
		id: infoImageContainer
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.top
		height: infoImage.implicitHeight
		width: infoImage.implicitWidth

		states: State {
			name: "alternativePinLetter"
			when: imageChangeTimer.alternativeLetter

			PropertyChanges {
				opacity: 1
				target: pinPukVariantB
			}
		}
		transitions: Transition {
			NumberAnimation {
				duration: Constants.animation_duration
				easing.type: Easing.InOutQuad
				properties: "opacity"
			}
		}

		Image {
			id: infoImage
			anchors.fill: parent
			fillMode: Image.PreserveAspectFit
			source: passwordType === PasswordType.CAN ? "qrc:///images/id_card.png" : passwordType === PasswordType.REMOTE_PIN ? "qrc:///images/phone_to_pc.svg" : passwordType === PasswordType.SMART_PIN || passwordType === PasswordType.NEW_SMART_PIN || passwordType === PasswordType.SMART_BLOCKING_CODE ? "" : "qrc:///images/pin_letter_pinpuk_same_page.svg"
			sourceSize.width: passwordType === PasswordType.CAN ? 400 * scaleFactorCan : 200 * scaleFactorGeneral
			width: parent.width

			Rectangle {
				readonly property real leftEdge: (parent.width - parent.paintedWidth) / 2.0
				readonly property real topEdge: (parent.height - parent.paintedHeight) / 2.0

				anchors.left: parent.left
				anchors.leftMargin: parent.paintedWidth * 0.7475 + leftEdge
				anchors.top: parent.top
				anchors.topMargin: parent.paintedWidth * 0.4175 + topEdge
				border.color: Constants.red
				border.width: Math.max(1, parent.paintedWidth * 0.015)
				color: Style.color.transparent
				height: parent.paintedWidth * 0.0625
				visible: passwordType === PasswordType.CAN
				width: parent.paintedWidth * 0.2
			}
		}
		Image {
			id: pinPukVariantB
			anchors.fill: parent
			fillMode: Image.PreserveAspectFit
			opacity: 0.0
			source: "qrc:///images/pin_letter_pinpuk_different_page.svg"
			sourceSize.width: 200 * scaleFactorGeneral
			visible: passwordType === PasswordType.PIN || passwordType === PasswordType.PUK
			width: parent.width
		}
	}
	GText {
		id: imageDescriptionText
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter
		text: (imageChangeTimer.alternativeLetter ?
			//: LABEL
			qsTr("ID card PIN/PUK on different pages") :
			//: LABEL
			qsTr("ID card PIN/PUK on the same page"))
		textStyle: Style.text.normal
		visible: passwordType === PasswordType.PIN || passwordType === PasswordType.PUK
		width: root.width
		wrapMode: Text.Wrap
	}
}
