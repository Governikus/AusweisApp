/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Type.SettingsModel

GPane {
	signal showDiagnosis
	signal showLogs

	spacing: Constants.component_spacing

	GMenuItem {
		Layout.fillWidth: true
		//: LABEL DESKTOP
		buttonText: qsTr("Show system data")
		iconSource: "qrc:/images/info.svg"

		//: LABEL DESKTOP
		title: qsTr("System data")

		onClicked: showDiagnosis()
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GMenuItem {
		Layout.fillWidth: true
		//: LABEL DESKTOP
		buttonText: qsTr("Show logs")
		iconSource: "qrc:/images/desktop/list_icon.svg"

		//: LABEL DESKTOP
		title: qsTr("Logs")

		onClicked: showLogs()
	}
}
