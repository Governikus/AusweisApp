/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Animations
import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.Type

FlickableSectionPage {
	id: baseItem

	property bool isTransportPin: false
	required property int passwordType

	signal continueClicked

	spacing: Constants.component_spacing

	AnimationLoader {
		id: animation

		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Constants.component_spacing
		animated: false
		symbol: EnterPasswordAnimation.Symbol.CHECK
		type: {
			switch (passwordType) {
			case NumberModel.PasswordType.CAN:
				return AnimationLoader.Type.ENTER_CAN;
			case NumberModel.PasswordType.PUK:
				return AnimationLoader.Type.ENTER_PUK;
			default:
				return AnimationLoader.Type.NONE;
			}
		}
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		activeFocusOnTab: true
		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (passwordType) {
			case NumberModel.PasswordType.CAN:
				//: INFO ALL_PLATFORMS
				return qsTr("CAN is correct");
			case NumberModel.PasswordType.PUK:
				//: INFO ALL_PLATFORMS
				return qsTr("PUK is correct");
			default:
				return "";
			}
		}
		textStyle: Style.text.headline

		FocusFrame {
		}
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		activeFocusOnTab: true
		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (passwordType) {
			case NumberModel.PasswordType.CAN:
				//: INFO ALL_PLATFORMS
				return isTransportPin ? "%1<br/><br/>%2".arg(qsTr("Please enter your <b>5-digit Transport PIN</b>. <b>This is your last attempt</b>.")).arg(qsTr("If you enter a wrong Transport PIN again, the PIN will be blocked. This block may only be removed using the PUK.")) :
				//: INFO ALL_PLATFORMS
				"%1<br/><br/>%2".arg(qsTr("Please enter your <b>6-digit ID card PIN</b>. <b>This is your last attempt</b>.")).arg(qsTr("If you enter a wrong ID Card PIN again, the PIN will be blocked. This block may only be removed using the PUK."));
			case NumberModel.PasswordType.PUK:
				//: INFO ALL_PLATFORMS
				return isTransportPin ? qsTr("You now have <b>3 more attempts</b> to enter your correct <b>Transport PIN</b>.") :
				//: INFO ALL_PLATFORMS
				qsTr("You now have <b>3 more attempts</b> to enter your correct <b>ID card PIN</b>.");
			default:
				return "";
			}
		}

		FocusFrame {
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GButton {
		id: buttonContinue

		Layout.alignment: Qt.AlignHCenter
		icon.source: "qrc:///images/material_arrow_right.svg"
		layoutDirection: Qt.RightToLeft

		//: LABEL ALL_PLATFORMS
		text: qsTr("Continue")
		tintIcon: true
		visible: text !== ""

		onClicked: continueClicked()
		onFocusChanged: if (focus)
			baseItem.positionViewAtItem(this)
	}
}
