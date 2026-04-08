/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.MultiInfoView
import Governikus.View
import Governikus.Style
import Governikus.Type

FlickableSectionPage {
	id: root

	required property string inputError
	property bool isTransportPin: false
	required property int passwordType
	required property int returnCode
	property alias titleVisible: title.visible

	signal continueClicked
	signal passwordInfoRequested

	spacing: Style.dimens.pane_spacing

	MultiInfoData {
		id: infoData

		contentType: {
			if (!root.isTransportPin) {
				return fromPasswordType(root.passwordType);
			}
			switch (root.returnCode) {
			case CardReturnCode.INVALID_CAN:
			case CardReturnCode.INVALID_PUK:
				return fromPasswordType(root.passwordType);
			default:
				return MultiInfoData.Type.TRANSPORT_PIN_NOT_WORKING;
			}
		}
	}
	AnimationLoader {
		id: animation

		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		animated: false
		symbol: Symbol.Type.ERROR
		type: {
			switch (root.returnCode) {
			case CardReturnCode.INVALID_PIN:
			case CardReturnCode.INVALID_PIN_2:
			case CardReturnCode.INVALID_PIN_3:
				return root.isTransportPin ? AnimationLoader.Type.TRANSPORT_PIN : AnimationLoader.Type.PIN;
			case CardReturnCode.INVALID_CAN:
				return AnimationLoader.Type.CAN;
			case CardReturnCode.INVALID_PUK:
				return AnimationLoader.Type.PUK;
			}
			switch (root.passwordType) {
			case NumberModel.PasswordType.NEW_PIN_CONFIRMATION:
			case NumberModel.PasswordType.NEW_PIN:
				return AnimationLoader.Type.NEW_PIN;
			}
			return AnimationLoader.Type.NONE;
		}
	}
	Heading {
		id: title

		text: {
			switch (root.returnCode) {
			case CardReturnCode.INVALID_CAN:
				//: ALL_PLATFORMS
				return qsTr("Wrong CAN");
			case CardReturnCode.INVALID_PUK:
				//: ALL_PLATFORMS
				return qsTr("Wrong PUK");
			case CardReturnCode.INVALID_PIN:
			case CardReturnCode.INVALID_PIN_2:
			case CardReturnCode.INVALID_PIN_3:
				return root.isTransportPin ?
				//: ALL_PLATFORMS
				qsTr("Wrong Transport PIN") :
				//: ALL_PLATFORMS
				qsTr("Wrong ID card PIN");
			}
			switch (root.passwordType) {
			case NumberModel.PasswordType.NEW_PIN_CONFIRMATION:
			case NumberModel.PasswordType.NEW_PIN:
				//: ALL_PLATFORMS
				return qsTr("Wrong new ID card PIN confirmation");
			}
			return "";
		}
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		text: root.inputError
	}
	MoreInformationLink {
		Layout.alignment: Qt.AlignHCenter
		text: infoData.linkText
		visible: {
			if (text === "") {
				return false;
			}

			switch (root.returnCode) {
			case CardReturnCode.INVALID_PIN_2:
			case CardReturnCode.INVALID_PIN_3:
			case CardReturnCode.INVALID_CAN:
			case CardReturnCode.INVALID_PUK:
				return true;
			default:
				return infoData.contentType === MultiInfoData.Type.TRANSPORT_PIN_NOT_WORKING;
			}
		}

		onClicked: root.passwordInfoRequested()
	}
	TintableIcon {
		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.pane_spacing
		source: "qrc:///images/can_info.svg"
		sourceSize.width: animation.width
		tintColor: Style.color.image
		tintEnabled: false
		visible: !root.isTransportPin && (root.returnCode === CardReturnCode.INVALID_PIN_2 || root.returnCode === CardReturnCode.INVALID_CAN)
	}
	GContinueButton {
		onClicked: root.continueClicked()
	}
}
