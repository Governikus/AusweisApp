/**
 * Copyright (c) 2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Style

GridLayout {
	readonly property real uniformButtonWidth: {
		let uniformWidth = 0;
		for (let i = 0; i < visibleChildren.length; ++i) {
			uniformWidth = Math.max(uniformWidth, visibleChildren[i].implicitWidth);
		}
		return uniformWidth;
	}

	Layout.alignment: Style.scanPatternAlignment
	Layout.topMargin: Style.dimens.pane_spacing
	columnSpacing: Style.dimens.pane_spacing
	columns: Style.is_layout_desktop ? visibleChildren.length : 1
	rowSpacing: Style.dimens.text_spacing
	uniformCellWidths: true
}
