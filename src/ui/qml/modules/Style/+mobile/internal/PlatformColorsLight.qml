/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */
import Governikus.Type

ColorsLight {
	pane.background.pressed: UiPluginModel.isChromeOS ? darker(pane.background.basic, 0.10) : pane.background.hovered
	paneSublevel.background.pressed: UiPluginModel.isChromeOS ? darker(paneSublevel.background.basic, 0.10) : paneSublevel.background.hovered
}
