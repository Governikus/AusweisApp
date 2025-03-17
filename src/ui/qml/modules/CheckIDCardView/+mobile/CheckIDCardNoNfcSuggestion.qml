/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Animations
import Governikus.Global

DecisionView {
	id: root

	property bool usedInOnboarding: false

	signal abortOnboarding
	signal connectSmartphone

	//: LABEL ANDROID IOS
	agreeButtonText: qsTr("Pair a NFC-enabled smartphone")
	descriptionTextsModel: {
		let textModel = [
			//: LABEL ANDROID IOS %1 will be replaced with the application name. The text between %2 and %3 will be emphasized.
			qsTr("If you want to use %1 on this device anyway, you can %2pair%3 another %2NFC-enabled smartphone%3 as a card reader.").arg(Qt.application.name).arg("<b>").arg("</b>")];

		if (usedInOnboarding) {
			//: LABEL ANDROID IOS The text between %1 and %2 will be emphasized.
			textModel.push(qsTr("We recommend that you set up your ID card on another smartphone. You can call the setup again at any time under %1Help > Setup%2.").arg("<b>").arg("</b>"));
		}

		//: LABEL ANDROID IOS %1 will be replaced with the application name. The text between %2 and %3 will be emphasized.
		textModel.push(qsTr("Alternatively you may use the %1 with an %2USB card reader%3 on your %2PC%3.").arg(Qt.application.name).arg("<b>").arg("</b>"));

		return textModel;
	}
	disagreeButtonHighlighted: usedInOnboarding
	//: LABEL ANDROID IOS
	disagreeButtonText: usedInOnboarding ? qsTr("Abort setup") : ""
	//: LABEL ANDROID IOS
	headlineText: qsTr("No NFC available")
	//: LABEL ANDROID IOS
	subtitleText: qsTr("This device cannot read the ID card")

	iconSourceComponent: AnimationLoader {
		animated: false
		symbol: Symbol.Type.ERROR
		type: AnimationLoader.NFC_RESULT
	}

	onAgreeClicked: root.connectSmartphone()
	onDisagreeClicked: root.abortOnboarding()
}
