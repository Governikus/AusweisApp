/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
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
		Back
	}

	readonly property color pressColor: Qt.darker(Style.color.textTitle.basic, Style.color.highlightDarkerFactor)
	property int type: NavigationAction.Action.Cancel

	colorStyle: Style.color.linkTitle
	height: if (parent)
		parent.height
	horizontalPadding: 0
	icon.source: root.type === NavigationAction.Action.Cancel ? "qrc:///images/material_close.svg" : "qrc:///images/material_arrow_back.svg"
	iconSize: Style.dimens.small_icon_size * 1.5
	text: root.type === NavigationAction.Action.Cancel ? qsTr("Cancel") : qsTr("Back")
	tintIcon: true
	verticalPadding: 0
	visible: ApplicationModel.currentWorkflow !== ApplicationModel.Workflow.NONE
}
