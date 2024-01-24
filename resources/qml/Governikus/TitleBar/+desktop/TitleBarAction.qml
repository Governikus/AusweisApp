/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
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
	property bool showHelp: true
	property bool showSettings: false

	background: null
	textDisabledColor: Style.color.text_subline_disabled
	textStyle: Style.text.navigation
	tintIcon: true
	verticalPadding: 0
}
