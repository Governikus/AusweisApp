/**
 * Copyright (c) 2021-2026 Governikus Service GmbH, Germany
 */

import QtQuick

import Governikus.ResultView

ResultView {
	property alias model: resultRepeater.model
	required property int pluginType
	required property int result

	Repeater {
		id: resultRepeater
	}
}
