/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

Item {
	readonly property int component_spacing: plugin.scaleFactor * 30
	readonly property int groupbox_spacing: plugin.scaleFactor * 20
	readonly property bool is_desktop: true
	readonly property int pane_padding: plugin.scaleFactor * 30
	readonly property int pane_spacing: plugin.scaleFactor * 30
	readonly property int subtext_spacing: Math.max(1, plugin.scaleFactor * 3)
	readonly property int text_spacing: plugin.scaleFactor * 10
}
