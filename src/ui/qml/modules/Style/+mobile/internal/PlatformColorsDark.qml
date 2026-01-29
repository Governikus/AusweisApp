/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */
import Governikus.Type

ColorsDark {
	pane.background.pressed_unchecked: UiPluginModel.isChromeOS ? lighter(pane.background.basic_unchecked, 0.10) : pane.background.hovered_unchecked
	paneSublevel.background.pressed_unchecked: UiPluginModel.isChromeOS ? lighter(paneSublevel.background.basic_unchecked, 0.10) : paneSublevel.background.hovered_unchecked
}
