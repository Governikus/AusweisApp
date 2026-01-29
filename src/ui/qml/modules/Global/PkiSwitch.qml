/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQml
import QtQuick

import Governikus.Style
import Governikus.Type

MouseArea {
	id: root

	Accessible.ignored: true

	onClicked: {
		d.counter += 1;
		switch (d.counter) {
		case 7:
		case 8:
		case 9:
			if (Style.is_layout_desktop) {
				//: DESKTOP Used in notifications when the user taps the icon
				ApplicationModel.showFeedback(qsTr("%1 more clicks to toggle the environment (prod/test) for integrated functions.").arg(10 - d.counter), true);
			} else {
				//: MOBILE Used in notifications when the user taps the icon
				ApplicationModel.showFeedback(qsTr("%1 more taps to toggle the environment (prod/test) for integrated functions.").arg(10 - d.counter), true);
			}
			break;
		case 10:
			SettingsModel.useSelfauthenticationTestUri = !SettingsModel.useSelfauthenticationTestUri;
			if (SettingsModel.useSelfauthenticationTestUri) {
				//: ALL_PLATFORMS Used in notifications when the user taps the icon
				ApplicationModel.showFeedback(qsTr("Testmode for the integrated functions activated."), true);
			} else {
				//: ALL_PLATFORMS Used in notifications when the user taps the icon
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
