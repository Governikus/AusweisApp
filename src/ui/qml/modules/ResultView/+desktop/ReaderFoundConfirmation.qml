/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Animations
import Governikus.ResultView
import Governikus.TitleBar

ResultView {
	id: root

	enum ReaderType {
		REMOTE,
		PCSC
	}

	required property int type

	animationSymbol: Symbol.Type.CHECK
	animationType: switch (type) {
	case ReaderFoundConfirmation.ReaderType.PCSC:
		return AnimationLoader.Type.WAIT_FOR_READER;
	case ReaderFoundConfirmation.ReaderType.REMOTE:
		return AnimationLoader.Type.WAIT_FOR_SAC;
	}

	//: LABEL DESKTOP
	buttonText: qsTr("Continue")
	text: switch (type) {
	case ReaderFoundConfirmation.ReaderType.PCSC:
		//: LABEL DESKTOP
		return qsTr("Found new USB card reader that is suitable for the ID card. The workflow may now be continued.");
	case ReaderFoundConfirmation.ReaderType.REMOTE:
		//: LABEL DESKTOP
		return qsTr("Found new smartphone as card reader that is suitable for the ID card. The workflow may now be continued.");
	}
	title: root.title

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Close

		onNavigationActionClicked: root.leaveView()
	}
}
