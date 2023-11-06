/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

Item {
	id: root

	enum Type {
		NONE,
		LETTER,
		LETTER_PUK,
		CAN,
		SMART,
		NO_PIN,
		PIN
	}

	property int imageType: PasswordInfoImage.Type.LETTER
	property real scaleFactorGeneral: 0.8

	height: root.implicitHeight
	implicitHeight: infoImageContainer.height
	implicitWidth: infoImageContainer.width
	visible: infoImage.source.toString() !== ""
	width: root.implicitWidth

	Item {
		id: infoImageContainer

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.top
		height: infoImage.implicitHeight
		width: infoImage.implicitWidth

		TintableIcon {
			id: infoImage

			anchors.fill: parent
			source: switch (imageType) {
			case PasswordInfoImage.Type.CAN:
				return "qrc:///images/can.svg";
			case PasswordInfoImage.Type.SMART:
				return "";
			case PasswordInfoImage.Type.LETTER_PUK:
				return "qrc:///images/puk_%1.svg".arg(Style.currentTheme.name);
			case PasswordInfoImage.Type.NO_PIN:
				return "qrc:///images/pin_unknown.svg";
			case PasswordInfoImage.Type.PIN:
				return "qrc:///images/pin_person.svg";
			default:
				return "qrc:///images/transportpin_%1.svg".arg(Style.currentTheme.name);
			}
			sourceSize.width: 200 * scaleFactorGeneral
			tintColor: Style.color.control
			tintEnabled: imageType === PasswordInfoImage.Type.NO_PIN || imageType === PasswordInfoImage.Type.CAN || imageType === PasswordInfoImage.Type.PIN
			width: parent.width
		}
	}
}
