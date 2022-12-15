/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0

ColumnLayout {
	spacing: Constants.component_spacing

	MoreViewMenuItem {
		Layout.fillWidth: true
		//: LABEL DESKTOP
		buttonText: qsTr("Show diagnosis")
		//: LABEL DESKTOP
		description: qsTr("You can view and save the diagnosis information of the AusweisApp2 and your system here.")
		iconSource: "qrc:/images/desktop/material_highlight.svg"

		//: LABEL DESKTOP
		title: qsTr("Diagnosis")

		onClicked: sectionPage.activeSubView = MoreView.SubViews.Diagnosis
	}
	GSeparator {
		Layout.fillWidth: true
	}
	MoreViewMenuItem {
		Layout.fillWidth: true
		//: LABEL DESKTOP
		buttonText: qsTr("Show logs")
		//: LABEL DESKTOP
		description: qsTr("Do you want to view the logs of %1?").arg(Qt.application.name)
		iconSource: "qrc:/images/desktop/material_view_list.svg"

		//: LABEL DESKTOP
		title: qsTr("Logs")

		onClicked: sectionPage.activeSubView = MoreView.SubViews.ApplicationLog
	}
	GSeparator {
		Layout.fillWidth: true
	}
	MoreViewMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/report".arg(SettingsModel.language)
		//: LABEL DESKTOP
		description: qsTr("Did you find a bug? Please help us by sending us the log file together with a description of the error.")
		iconSource: "qrc:/images/desktop/material_bug_report.svg"

		//: LABEL DESKTOP
		title: qsTr("Report error")

		onClicked: Qt.openUrlExternally(buttonTooltip)
	}
}
