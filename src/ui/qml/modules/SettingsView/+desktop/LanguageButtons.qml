/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

GridLayout {
	id: root

	signal buttonClicked

	columnSpacing: Style.dimens.pane_spacing
	rowSpacing: Style.dimens.pane_spacing

	GRepeater {
		id: repeater

		delegate: LocationButton {
			Layout.fillWidth: true
			Layout.preferredWidth: Style.is_layout_desktop ? repeater.maxItemWidth : -1

			onClicked: root.buttonClicked()
		}
		model: LanguageButtonData {
		}
	}
}
