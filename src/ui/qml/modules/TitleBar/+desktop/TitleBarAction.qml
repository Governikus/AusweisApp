/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

GLink {
	property var customSettingsHandler
	property Item customSubAction: Item {
		visible: false
	}
	property bool rootEnabled: true
	readonly property var settingsHandler: customSettingsHandler ? customSettingsHandler : function () {}
	property bool showSettings: false

	colorStyle: Style.color.linkNavigation
	iconSize: Style.dimens.icon_size
	textStyle: Style.text.navigation
	verticalPadding: 0
}
