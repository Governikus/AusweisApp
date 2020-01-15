/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0

ColumnLayout {

	spacing: Constants.component_spacing

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Diagnosis") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		description: qsTr("You can view and save the diagnosis information of the AusweisApp2 and your system here.") + SettingsModel.translationTrigger
		iconSource: "qrc:/images/desktop/info_diagnosis.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Show diagnosis") + SettingsModel.translationTrigger
		onClicked: sectionPage.activeSubView = MoreView.SubViews.Diagnosis
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Application log") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		description: qsTr("You can view the logs of the AusweisApp2 here.") + SettingsModel.translationTrigger
		iconSource: "qrc:/images/desktop/info_application_log.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Show logs") + SettingsModel.translationTrigger
		onClicked: sectionPage.activeSubView = MoreView.SubViews.ApplicationLog
	}

}
