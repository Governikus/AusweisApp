/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

ColumnLayout {
	id: root

	property int statusCode: -1
	property alias title: titleText.text

	signal linkAboutToOpen

	spacing: Style.dimens.pane_spacing
	visible: title !== "" && statusCode !== GlobalStatusCode.Card_ValidityVerificationFailed

	GText {
		id: titleText

		Layout.alignment: Qt.AlignLeft
		text: d.getTitle()
		textStyle: Style.text.subline
	}
	Hint {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		buttonText: PinResetInformationModel.resetPinWithPRSActionText
		linkToOpen: PinResetInformationModel.pinResetUrl
		objectName: "PRSHint"
		text: d.getPRSHintText()
		title: PinResetInformationModel.resetPinWithPRSHintTitle
		visible: text !== ""

		onLinkAboutToOpen: root.linkAboutToOpen()
	}
	Hint {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		buttonText: PinResetInformationModel.resetPinAtAuthorityActionText
		linkToOpen: PinResetInformationModel.administrativeSearchUrl
		objectName: "AuthorityHint"
		text: d.getAuthorityHintText()
		title: PinResetInformationModel.resetPinAtAuthorityHintTitle

		onLinkAboutToOpen: root.linkAboutToOpen()
	}
	QtObject {
		id: d

		property bool hasPRS: PinResetInformationModel.hasPinResetService
		property string resetPinWithPRSHint: PinResetInformationModel.resetPinWithPRSHint

		function getAuthorityHintText() {
			if (root.statusCode === GlobalStatusCode.Card_Pin_Deactivated)
				return PinResetInformationModel.activateOnlineFunctionAtAuthorityHint;

			return PinResetInformationModel.resetPinAtAuthorityHint;
		}
		function getPRSHintText() {
			if (!hasPRS)
				return "";

			if (root.statusCode === GlobalStatusCode.Card_Pin_Deactivated)
				return PinResetInformationModel.activateOnlineFunctionForPRSHint;

			return resetPinWithPRSHint;
		}
		function getTitle() {
			if (root.statusCode === GlobalStatusCode.Card_Pin_Deactivated)
				//: ALL_PLATFORMS Hint title to assist the user on how to set a new PIN
				return qsTr("How do I activate the eID function?");

			if (root.statusCode === GlobalStatusCode.Card_Puk_Blocked)
				//: ALL_PLATFORMS Hint title to assist the user on how to set a new PIN
				return qsTr("My PUK is used up. How do I set a new PIN?");

			return "";
		}

		objectName: "Private"
	}
}
