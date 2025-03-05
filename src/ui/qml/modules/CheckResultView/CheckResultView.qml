/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.ResultView

ResultView {
	id: root

	property alias model: resultRepeater.model
	required property int pluginType
	required property int result

	Repeater {
		id: resultRepeater

	}
}
