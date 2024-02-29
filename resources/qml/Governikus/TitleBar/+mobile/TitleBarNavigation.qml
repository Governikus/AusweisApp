/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

TitleBarAction {
	id: baseItem

	property int action: navigationAction.Action.None

	Accessible.name: text
	icon: {
		switch (baseItem.action) {
		case NavigationAction.Action.Cancel:
			return "qrc:///images/material_close.svg";
		case NavigationAction.Action.Back:
			return "qrc:///images/mobile/material_arrow_left.svg";
		default:
			return "";
		}
	}
	text: {
		switch (baseItem.action) {
		//: LABEL ANDROID IOS
		case NavigationAction.Action.Cancel:
			return qsTr("Cancel");
		//: LABEL ANDROID IOS
		case NavigationAction.Action.Back:
			return qsTr("Back");
		default:
			return "";
		}
	}
}
