/**
 * Copyright (c) 2023-2026 Governikus Service GmbH, Germany
 */

import Governikus.Type

import QtQuick.Effects

MultiEffect {
	shadowEnabled: true
	shadowOpacity: UiPluginModel.qtVersion === "6.8.0" ? 0.4 : 0.15
	shadowScale: 1.025
	shadowVerticalOffset: 7
}
