/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel

GButton {
	id: root

	enum Action {
		Cancel,
		Back
	}

	readonly property color pressColor: Qt.darker(Style.color.text_title, Constants.highlightDarkerFactor)
	property int type: NavigationAction.Action.Cancel

	background: null
	height: if (parent)
		parent.height
	horizontalPadding: 0
	icon.source: root.type === NavigationAction.Action.Cancel ? "qrc:///images/material_close.svg" : "qrc:///images/material_arrow_back.svg"
	iconSize: Style.dimens.small_icon_size * 1.5
	text: root.type === NavigationAction.Action.Cancel ? qsTr("Cancel") : qsTr("Back")
	textDisabledColor: Style.color.text_disabled
	textHighlightColor: plugin.highContrastEnabled ? Style.color.control_pressed : Style.color.control_content_pressed
	textStyle: Style.text.normal
	tintIcon: true
	verticalPadding: 0
	visible: ApplicationModel.currentWorkflow !== ApplicationModel.WORKFLOW_NONE
}
