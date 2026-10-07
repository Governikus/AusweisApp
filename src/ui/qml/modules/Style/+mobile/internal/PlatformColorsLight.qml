/**
 * Copyright (c) 2024-2026 Governikus Service GmbH, Germany
 */

import Governikus.Type

ColorsLight {
	pane.background.pressed_unchecked: UiPluginModel.isChromeOS ? darker(pane.background.basic_unchecked, 0.10) : pane.background.hovered_unchecked
	paneSublevel.background.pressed_unchecked: UiPluginModel.isChromeOS ? darker(paneSublevel.background.basic_unchecked, 0.10) : paneSublevel.background.hovered_unchecked
}
