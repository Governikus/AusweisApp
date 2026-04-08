/**
 * Copyright (c) 2023-2026 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Animations
import Governikus.ResultView

ResultView {
	required property string deviceName
	required property string errorMessage

	animationSymbol: Symbol.Type.ERROR
	animationType: AnimationLoader.Type.SAC_CONNECTION
	//: ALL_PLATFORMS
	header: qsTr("Pairing failed")
	//: ALL_PLATFORMS
	subheader: qsTr("Pairing to \"%1\" failed").arg(deviceName)
	text: errorMessage
}
