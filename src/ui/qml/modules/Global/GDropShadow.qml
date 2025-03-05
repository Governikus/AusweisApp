/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Type

import QtQuick.Effects

MultiEffect {
	shadowEnabled: true
	shadowOpacity: UiPluginModel.qtVersion === "6.8.0" ? 0.4 : 0.15
	shadowScale: 1.025
	shadowVerticalOffset: 7
}
