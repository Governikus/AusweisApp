/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.Type

FlickableSectionPage {
	id: root

	property bool isTransportPin: false
	required property int passwordType

	signal continueClicked

	spacing: Style.dimens.pane_spacing

	Keys.onEnterPressed: continueClicked()
	Keys.onEscapePressed: continueClicked()
	Keys.onReturnPressed: continueClicked()

	GText {
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		text: header.text
		textStyle: Style.text.headline
		visible: subHeader.visible
	}
	AnimationLoader {
		id: animation

		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		animated: false
		symbol: Symbol.Type.CHECK
		type: {
			switch (root.passwordType) {
			case NumberModel.PasswordType.CAN:
				return AnimationLoader.CAN;
			case NumberModel.PasswordType.PUK:
				return AnimationLoader.PUK;
			case NumberModel.PasswordType.TRANSPORT_PIN:
				return AnimationLoader.TRANSPORT_PIN;
			default:
				return AnimationLoader.NONE;
			}
		}
	}
	GText {
		id: header

		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (root.passwordType) {
			case NumberModel.PasswordType.CAN:
				//: INFO ALL_PLATFORMS
				return qsTr("CAN is correct");
			case NumberModel.PasswordType.PUK:
				//: INFO ALL_PLATFORMS
				return qsTr("PUK is correct");
			case NumberModel.PasswordType.TRANSPORT_PIN:
				//: INFO ALL_PLATFORMS
				return qsTr("Transport PIN correct");
			default:
				return "";
			}
		}
		textStyle: Style.text.headline
		visible: !subHeader.visible
	}
	GText {
		id: subHeader

		//: LABEL ALL_PLATFORMS
		text: qsTr("Now set your personal ID card PIN")
		textStyle: Style.text.subline
		visible: root.passwordType === NumberModel.PasswordType.TRANSPORT_PIN
	}
	GText {
		Layout.alignment: subHeader.visible ? Qt.AlignLeft : Qt.AlignHCenter
		horizontalAlignment: subHeader.visible ? Text.AlignLeft : Text.AlignHCenter
		text: {
			switch (root.passwordType) {
			case NumberModel.PasswordType.CAN:
				//: INFO ALL_PLATFORMS
				return root.isTransportPin ? "%1<br/><br/>%2".arg(qsTr("Please enter your <b>5-digit Transport PIN</b>. <b>This is your last attempt</b>.")).arg(qsTr("If you enter a wrong Transport PIN again, the PIN will be blocked. This block may only be removed using the PUK.")) :
				//: INFO ALL_PLATFORMS
				"%1<br/><br/>%2".arg(qsTr("Please enter your <b>6-digit ID card PIN</b>. <b>This is your last attempt</b>.")).arg(qsTr("If you enter a wrong ID Card PIN again, the PIN will be blocked. This block may only be removed using the PUK."));
			case NumberModel.PasswordType.PUK:
				//: INFO ALL_PLATFORMS
				return root.isTransportPin ? qsTr("You now have <b>3 more attempts</b> to enter your correct <b>Transport PIN</b>.") :
				//: INFO ALL_PLATFORMS
				qsTr("You now have <b>3 more attempts</b> to enter your correct <b>ID card PIN</b>.");
			case NumberModel.PasswordType.TRANSPORT_PIN:
				return "%1<br>%2"
				//: INFO ALL_PLATFORMS
				.arg(qsTr("The Transport PIN is now replaced by your personal 6-digit ID card PIN."))
				//: INFO ALL_PLATFORMS The text between %1 and %2 will be emphasized.
				.arg(qsTr("Afterwards the %1Transport PIN is no longer required.%2").arg("<b>").arg("</b>"));
			default:
				return "";
			}
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GContinueButton {
		onClicked: root.continueClicked()
	}
}
