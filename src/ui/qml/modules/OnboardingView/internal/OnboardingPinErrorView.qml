/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
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

	//: LABEL ALL_PLATFORMS
	agreeButtonText: ChangePinModel.pukInoperative ? "" : qsTr("Try again")
	descriptionTextsModel: [ChangePinModel.resultString]
	disagreeButtonHighlighted: ChangePinModel.pukInoperative

	//: LABEL ALL_PLATFORMS
	disagreeButtonText: qsTr("Abort setup")

	customContentSourceComponent: Hint {
		Layout.fillWidth: true
		buttonText: ChangePinModel.statusHintActionText
		text: ChangePinModel.statusHintText
		title: ChangePinModel.statusHintTitle
		visible: text !== ""

		onClicked: ChangePinModel.invokeStatusHintAction()
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

	onAgreeClicked: root.tryAgain()
	onDisagreeClicked: ChangePinModel.pukInoperative ? root.pukBlocked() : root.abort()
}
