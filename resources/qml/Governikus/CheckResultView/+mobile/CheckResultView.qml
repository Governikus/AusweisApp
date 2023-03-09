/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.ResultView 1.0

ResultView {
	id: root

	property alias model: resultRepeater.model
	property int result

	ColumnLayout {
		width: parent.width

		Repeater {
			id: resultRepeater
		}
	}
}
