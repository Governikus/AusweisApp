/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: root
	enum Type {
		NONE,
		LETTER,
		LETTER_PUK,
		CAN,
		SMARTPHONE_AS_CARD_READER,
		SMART,
		NO_PIN,
		PIN
	}

	property int imageType: PasswordInfoImage.Type.LETTER
	property real scaleFactorCan: 1
	property real scaleFactorGeneral: 1

	height: root.implicitHeight
	implicitHeight: infoImageContainer.height
	implicitWidth: infoImageContainer.width
	visible: infoImage.source.toString() !== ""
	width: root.implicitWidth

	Item {
		id: infoImageContainer
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.top
		height: imageType == PasswordInfoImage.Type.CAN ? (Constants.is_desktop ? 255 : 159) * scaleFactorCan : infoImage.implicitHeight
		width: imageType == PasswordInfoImage.Type.CAN ? (Constants.is_desktop ? 400 : 250) * scaleFactorCan : infoImage.implicitWidth

		TintableIcon {
			id: infoImage
			anchors.fill: parent
			source: switch (imageType) {
			case PasswordInfoImage.Type.CAN:
				return "qrc:///images/id_card.png";
			case PasswordInfoImage.Type.SMARTPHONE_AS_CARD_READER:
				return "qrc:///images/phone_to_pc.svg";
			case PasswordInfoImage.Type.SMART:
				return "";
			case PasswordInfoImage.Type.LETTER_PUK:
				return "qrc:///images/pin_letter_pinpuk_red_bar_puk.svg";
			case PasswordInfoImage.Type.NO_PIN:
				return "qrc:///images/material_live_help.svg";
			case PasswordInfoImage.Type.PIN:
				return "qrc:///images/pin_person.svg";
			default:
				return "qrc:///images/pin_letter_pinpuk_red_bar.svg";
			}
			sourceSize.width: {
				switch (imageType) {
				case PasswordInfoImage.Type.CAN:
					return undefined;
				case PasswordInfoImage.Type.NO_PIN:
					return 100 * scaleFactorGeneral;
				default:
					return 200 * scaleFactorGeneral;
				}
			}
			tintColor: Style.color.accent
			tintEnabled: imageType == PasswordInfoImage.Type.NO_PIN
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
				visible: imageType == PasswordInfoImage.Type.CAN
				width: parent.paintedWidth * 0.2
			}
		}
	}
}
