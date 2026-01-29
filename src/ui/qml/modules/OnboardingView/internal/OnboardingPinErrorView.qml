/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.Type

DecisionView {
	id: root

	signal abort
	signal pukBlocked
	signal tryAgain

	descriptionTextsModel: [ChangePinModel.resultString]
	primaryButton.text: {
		return ChangePinModel.pukInoperative ?
		//: ALL_PLATFORMS
		qsTr("Abort setup") :
		//: ALL_PLATFORMS
		qsTr("Try again");
	}
	//: ALL_PLATFORMS
	secondaryButton.text: ChangePinModel.pukInoperative ? "" : qsTr("Abort setup")

	customContentSourceComponent: ColumnLayout {
		spacing: Style.dimens.pane_spacing

		GText {
			text: ChangePinModel.statusHintBoxesTitle
			textStyle: Style.text.subline
			visible: text !== ""
		}
		Hint {
			Layout.fillWidth: true
			buttonText: ChangePinModel.statusHintActionText
			text: ChangePinModel.statusHintText
			title: ChangePinModel.statusHintTitle
			visible: text !== ""

			onClicked: ChangePinModel.invokeStatusHintAction()
		}
		Hint {
			Layout.fillWidth: true
			buttonText: PinResetInformationModel.resetPinAtAuthorityActionText
			linkToOpen: PinResetInformationModel.administrativeSearchUrl
			text: Utils.getSecondPRSHintText(ChangePinModel.statusCode)
			title: PinResetInformationModel.resetPinAtAuthorityHintTitle
			visible: text !== ""
		}
	}
	iconSourceComponent: Column {
		TintableIcon {
			id: customIcon

			Layout.alignment: Qt.AlignHCenter
			sourceSize.height: Style.dimens.header_icon_size
			tintEnabled: false
			visible: source.toString() !== ""
		}
		AnimationLoader {
			id: animatedIcon

			Layout.alignment: Qt.AlignHCenter
			animated: false
			type: ChangePinModel.statusCodeAnimation
		}
	}

	onPrimaryButtonClicked: ChangePinModel.pukInoperative ? root.pukBlocked() : root.tryAgain()
	onSecondaryButtonClicked: root.abort()
}
