/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0

ColumnLayout {

	spacing: Constants.component_spacing

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Diagnosis")
		//: LABEL DESKTOP_QML
		description: qsTr("You can view and save the diagnosis information of the AusweisApp2 and your system here.")
		iconSource: "qrc:/images/desktop/material_highlight.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Show diagnosis")
		onClicked: sectionPage.activeSubView = MoreView.SubViews.Diagnosis
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Logs")
		//: LABEL DESKTOP_QML
		description: qsTr("Do you want to view the logs of %1?").arg(Qt.application.name)
		iconSource: "qrc:/images/desktop/material_view_list.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Show logs")
		onClicked: sectionPage.activeSubView = MoreView.SubViews.ApplicationLog
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Report error")
		//: LABEL DESKTOP_QML
		description: qsTr("Did you find a bug? Please help us by sending us the log file together with a description of the error.")
		iconSource: "qrc:/images/desktop/material_bug_report.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/report".arg(SettingsModel.language)
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		onClicked: Qt.openUrlExternally(buttonTooltip)
	}

}
