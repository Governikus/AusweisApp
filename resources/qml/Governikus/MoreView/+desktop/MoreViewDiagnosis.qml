/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
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

	MoreViewMenuItem {
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
	MoreViewMenuItem {
		Layout.fillWidth: true
		//: LABEL DESKTOP
		buttonText: qsTr("Show logs")
		iconSource: "qrc:/images/desktop/list_icon.svg"

		//: LABEL DESKTOP
		title: qsTr("Logs")

		onClicked: showLogs()
	}
}
