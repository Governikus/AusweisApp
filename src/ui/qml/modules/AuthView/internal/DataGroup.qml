/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

ColumnLayout {
	property alias chat: repeater.model
	readonly property alias count: repeater.count
	property alias title: dataTitle.text

	Accessible.focusable: true
	Accessible.ignored: title === "" && !Style.is_layout_desktop
	Accessible.name: title
	Accessible.role: Accessible.Grouping
	spacing: Style.dimens.pane_spacing
	visible: count > 0

	onFocusChanged: if (focus)
		Utils.positionViewAtItem(this)

	Subheading {
		id: dataTitle

	}
	GridLayout {
		columnSpacing: Style.dimens.pane_spacing
		columns: Style.is_layout_desktop ? 2 : 1

		GRepeater {
			id: repeater

			DataGroupDelegate {
				Layout.fillWidth: true
				count: repeater.count
			}
		}
	}
}
