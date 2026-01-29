/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick.Layouts

import Governikus.Global

GPane {
	id: root

	signal showDiagnosis
	signal showLogs

	GMenuItem {
		Layout.fillWidth: true
		//: DESKTOP
		buttonText: qsTr("Show system data")
		iconSource: "qrc:/images/info.svg"

		//: DESKTOP
		title: qsTr("System data")

		onClicked: root.showDiagnosis()
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GMenuItem {
		Layout.fillWidth: true
		//: DESKTOP
		buttonText: qsTr("Show logs")
		iconSource: "qrc:/images/desktop/list_icon.svg"

		//: DESKTOP
		title: qsTr("Logs")

		onClicked: root.showLogs()
	}
}
