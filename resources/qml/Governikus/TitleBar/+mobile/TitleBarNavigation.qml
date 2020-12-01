/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtGraphicalEffects 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

TitleBarAction {
	id: baseItem

	Accessible.role: Accessible.Button
	Accessible.name: text

	icon: {
		if (Constants.is_layout_ios) {
			return baseItem.state === "back" ? "qrc:///images/ios/material_arrow_left.svg" : ""
		}

		switch(baseItem.state) {
			case "cancel": return "qrc:///images/material_close.svg"
			case "back": return "qrc:///images/mobile/material_arrow_back.svg"
			default: return ""
		}
	}
	text: {
		if (Constants.is_layout_android) {
			return ""
		}

		switch(baseItem.state) {
			//: LABEL ANDROID IOS
			case "cancel": return qsTr("Cancel")
			//: LABEL ANDROID IOS
			case "edit": return qsTr("Edit")
			//: LABEL ANDROID IOS
			case "back": return qsTr("Back")
			default: return ""
		}
	}
}
