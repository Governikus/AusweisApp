/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */
import Governikus.Type

ColorsDark {
	pane.background.pressed: UiPluginModel.isChromeOS ? lighter(pane.background.basic, 0.10) : pane.background.hovered
	paneSublevel.background.pressed: UiPluginModel.isChromeOS ? lighter(paneSublevel.background.basic, 0.10) : paneSublevel.background.hovered
}
