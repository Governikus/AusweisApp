/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

TitleBarAction {
	id: baseItem

	property int action
	readonly property string rawText: {
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

	Accessible.name: rawText
	icon: {
		if (Constants.is_layout_ios) {
			return baseItem.action === NavigationAction.Action.Back ? "qrc:///images/ios/material_arrow_left.svg" : "";
		}
		switch (baseItem.action) {
		case NavigationAction.Action.Cancel:
			return "qrc:///images/material_close.svg";
		case NavigationAction.Action.Back:
			return "qrc:///images/mobile/material_arrow_back.svg";
		default:
			return "";
		}
	}
	text: {
		if (Constants.is_layout_android) {
			return "";
		}
		return rawText;
	}
}
