/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Animations
import Governikus.ResultView

ResultView {
	id: root

	required property string deviceName
	required property string errorMessage

	animationSymbol: Symbol.Type.ERROR
	animationType: AnimationLoader.SAC_CONNECTION
	//: LABEL ALL_PLATFORMS
	header: qsTr("Pairing failed")
	//: LABEL ALL_PLATFORMS
	subheader: qsTr("Pairing to \"%1\" failed").arg(deviceName)
	text: errorMessage
}
