/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.View

GButton {
	property var customSettingsHandler
	property Item customSubAction: Item {
		visible: false
	}
	property bool rootEnabled: true
	readonly property var settingsHandler: customSettingsHandler ? customSettingsHandler : function () {}
	property bool showSettings: false

	background: null
	iconSize: Style.dimens.icon_size
	textDisabledColor: Style.color.text_title_disabled
	textHighlightColor: plugin.highContrastEnabled ? Style.color.control_pressed : Style.color.control_content_pressed
	textStyle: Style.text.navigation
	tintIcon: true
	verticalPadding: 0
}
