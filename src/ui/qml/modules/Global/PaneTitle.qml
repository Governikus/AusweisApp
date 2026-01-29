/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Style

BaseHeading {
	elide: Text.ElideRight
	maximumLineCount: {
		if (Style.is_layout_desktop) {
			return 1;
		}
	}
	textStyle: Style.is_layout_desktop ? Style.text.subline : Style.text.headline
	visible: text !== ""
}
