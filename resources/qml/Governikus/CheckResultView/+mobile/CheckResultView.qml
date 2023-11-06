/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.ResultView

ResultView {
	id: root

	property alias model: resultRepeater.model
	required property int result

	Repeater {
		id: resultRepeater

	}
}
