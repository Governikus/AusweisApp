/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

TitleBarAction {
	id: root

	property int navAction: NavigationAction.Action.None

	Accessible.name: text
	icon.source: {
		switch (root.navAction) {
		case NavigationAction.Action.Close:
		case NavigationAction.Action.Cancel:
			return "qrc:///images/material_close.svg";
		case NavigationAction.Action.Back:
			return "qrc:///images/mobile/material_arrow_left.svg";
		default:
			return "";
		}
	}
	text: {
		switch (root.navAction) {
		case NavigationAction.Action.Close:
			//: MOBILE
			return qsTr("Close");
		case NavigationAction.Action.Cancel:
			//: MOBILE
			return qsTr("Cancel");
		case NavigationAction.Action.Back:
			//: MOBILE
			return qsTr("Back");
		default:
			return "";
		}
	}
}
