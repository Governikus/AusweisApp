/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import QtQuick
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel

MouseArea {
	id: root

	required property string functionName

	Accessible.name: functionName
	Accessible.role: Accessible.Button

	Accessible.onPressAction: root.clicked(null)
	onClicked: {
		d.counter += 1;
		switch (d.counter) {
		case 7:
		case 8:
		case 9:
			//: INFO ANDROID IOS Used in notifications when the user taps the icon
			ApplicationModel.showFeedback(qsTr("%1 more presses to toggle the environment (prod/test) for integrated functions.").arg(10 - d.counter), true);
			break;
		case 10:
			SettingsModel.useSelfauthenticationTestUri = !SettingsModel.useSelfauthenticationTestUri;
			if (SettingsModel.useSelfauthenticationTestUri) {
				//: INFO ANDROID IOS Used in notifications when the user taps the icon
				ApplicationModel.showFeedback(qsTr("Testmode for the integrated functions activated."), true);
			} else {
				//: INFO ANDROID IOS Used in notifications when the user taps the icon
				ApplicationModel.showFeedback(qsTr("Testmode for the integrated functions deactivated."), true);
			}
			d.counter = 0;
			break;
		}
	}

	QtObject {
		id: d

		property int counter: 0
	}
}
