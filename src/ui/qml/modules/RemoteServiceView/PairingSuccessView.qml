/**
 * Copyright (c) 2023-2026 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Animations
import Governikus.ResultView

ResultView {
	id: root

	required property string deviceName

	animationSymbol: Symbol.Type.CHECK
	animationType: AnimationLoader.Type.SAC_CONNECTION
	//: ALL_PLATFORMS
	header: qsTr("Pairing successful")
	//: ALL_PLATFORMS
	subheader: qsTr("Your device \"%1\" was successfully paired").arg(deviceName)
	//: ALL_PLATFORMS The pairing with the SaC server was successful.
	text: qsTr("Your smartphone as card reader has been paired and can now be used to read your ID card when you start a process.") + "<br /><br />" +
	//: ALL_PLATFORMS The pairing with the SaC server was successful.
	qsTr("In the future, you only need to select \"Activate card reader\" in the Card reader menu on the smartphone to use your smartphone as a card reader for this device.")
}
