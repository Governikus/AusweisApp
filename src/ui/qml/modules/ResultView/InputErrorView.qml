/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Animations
import Governikus.Global
import Governikus.PasswordInfoView
import Governikus.View
import Governikus.Style
import Governikus.Type

FlickableSectionPage {
	id: baseItem

	required property string inputError
	property bool isTransportPin: false
	required property int passwordType
	required property int returnCode
	property alias titleVisible: title.visible

	signal continueClicked
	signal passwordInfoRequested

	spacing: Constants.component_spacing

	PasswordInfoData {
		id: infoData

		contentType: {
			if (!isTransportPin) {
				return fromPasswordType(passwordType);
			}
			switch (returnCode) {
			case CardReturnCode.INVALID_CAN:
			case CardReturnCode.INVALID_PUK:
				return fromPasswordType(passwordType);
			default:
				return PasswordInfoData.Type.TRANSPORT_PIN_NOT_WORKING;
			}
		}
	}
	AnimationLoader {
		id: animation

		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Constants.component_spacing
		animated: false
		symbol: EnterPasswordAnimation.Symbol.ERROR
		type: {
			switch (returnCode) {
			case CardReturnCode.INVALID_PIN:
			case CardReturnCode.INVALID_PIN_2:
			case CardReturnCode.INVALID_PIN_3:
				return isTransportPin ? AnimationLoader.Type.ENTER_TRANSPORT_PIN : AnimationLoader.Type.ENTER_PIN;
			case CardReturnCode.INVALID_CAN:
				return AnimationLoader.Type.ENTER_CAN;
			case CardReturnCode.INVALID_PUK:
				return AnimationLoader.Type.ENTER_PUK;
			}
			switch (passwordType) {
			case NumberModel.PasswordType.NEW_PIN_CONFIRMATION:
			case NumberModel.PasswordType.NEW_PIN:
			case NumberModel.PasswordType.NEW_SMART_PIN:
			case NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION:
				return AnimationLoader.Type.ENTER_NEW_PIN;
			}
			return AnimationLoader.Type.NONE;
		}
	}
	GText {
		id: title

		Layout.alignment: Qt.AlignHCenter
		activeFocusOnTab: true
		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (returnCode) {
			case CardReturnCode.INVALID_CAN:
				//: LABEL ALL_PLATFORMS
				return qsTr("Wrong CAN");
			case CardReturnCode.INVALID_PUK:
				//: LABEL ALL_PLATFORMS
				return qsTr("Wrong PUK");
			case CardReturnCode.INVALID_PIN:
			case CardReturnCode.INVALID_PIN_2:
			case CardReturnCode.INVALID_PIN_3:
				return isTransportPin ?
				//: LABEL ALL_PLATFORMS
				qsTr("Wrong Transport PIN") : smartEidUsed ?
				//: LABEL ALL_PLATFORMS
				qsTr("Wrong Smart-eID PIN") :
				//: LABEL ALL_PLATFORMS
				qsTr("Wrong ID card PIN");
			}
			switch (passwordType) {
			case NumberModel.PasswordType.NEW_PIN_CONFIRMATION:
			case NumberModel.PasswordType.NEW_PIN:
				//: LABEL ALL_PLATFORMS
				return qsTr("Wrong new ID card PIN confirmation");
			case NumberModel.PasswordType.NEW_SMART_PIN:
			case NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION:
				//: LABEL ALL_PLATFORMS
				return qsTr("Wrong new Smart-eID PIN confirmation");
			}
			return "";
		}
		textStyle: Style.text.headline

		FocusFrame {
		}
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		activeFocusOnTab: true
		horizontalAlignment: Text.AlignHCenter
		text: inputError

		FocusFrame {
		}
	}
	MoreInformationLink {
		Layout.alignment: Qt.AlignHCenter
		activeFocusOnTab: true
		text: infoData.linkText
		visible: text !== "" && (returnCode === CardReturnCode.INVALID_PUK || infoData.contentType === PasswordInfoData.Type.TRANSPORT_PIN_NOT_WORKING)

		onClicked: passwordInfoRequested()

		FocusFrame {
		}
	}
	TintableIcon {
		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Constants.component_spacing
		source: "qrc:///images/can_info.svg"
		sourceSize.width: animation.width
		tintColor: Style.color.image
		tintEnabled: false
		visible: !isTransportPin && (returnCode === CardReturnCode.INVALID_PIN_2 || returnCode === CardReturnCode.INVALID_CAN)
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GButton {
		Layout.alignment: Qt.AlignHCenter
		icon.source: "qrc:///images/material_arrow_right.svg"
		layoutDirection: Qt.RightToLeft

		//: LABEL ALL_PLATFORMS
		text: qsTr("Continue")
		tintIcon: true

		onClicked: continueClicked()
		onFocusChanged: if (focus)
			baseItem.positionViewAtItem(this)
	}
}
