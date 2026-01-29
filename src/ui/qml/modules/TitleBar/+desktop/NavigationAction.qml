/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.Type

GLink {
	id: root

	enum Action {
		None,
		Cancel,
		Back,
		Close
	}

	property int type: NavigationAction.Action.Cancel

	colorStyle: Style.color.linkTitle
	height: if (parent)
		parent.height
	horizontalPadding: 0
	icon.source: switch (root.type) {
	case NavigationAction.Action.Cancel:
	case NavigationAction.Action.Close:
		return "qrc:///images/material_close.svg";
	default:
		return "qrc:///images/material_arrow_back.svg";
	}
	iconSize: Style.dimens.small_icon_size * 1.5
	text: switch (root.type) {
	case NavigationAction.Action.Cancel:
		//: DESKTOP
		return qsTr("Cancel");
	case NavigationAction.Action.Close:
		//: DESKTOP
		return qsTr("Close");
	default:
		//: DESKTOP
		return qsTr("Back");
	}
	tintIcon: true
	verticalPadding: 0
	visible: ApplicationModel.currentWorkflow !== ApplicationModel.Workflow.NONE
}
