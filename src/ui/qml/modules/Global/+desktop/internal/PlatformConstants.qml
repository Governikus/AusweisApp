/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Type

Item {
	readonly property int component_spacing: UiPluginModel.scaleFactor * 30
	readonly property int control_horizontalPadding: UiPluginModel.scaleFactor * 30
	readonly property int control_verticalPadding: UiPluginModel.scaleFactor * 10
	readonly property int groupbox_spacing: UiPluginModel.scaleFactor * 20
	readonly property bool is_desktop: true
	readonly property int pane_border_highlight_width: UiPluginModel.scaleFactor * 4
	readonly property int pane_padding: UiPluginModel.scaleFactor * 30
	readonly property int pane_spacing: UiPluginModel.scaleFactor * 30
	readonly property int subtext_spacing: Math.max(1, UiPluginModel.scaleFactor * 3)
	readonly property int text_spacing: UiPluginModel.scaleFactor * 10
}
