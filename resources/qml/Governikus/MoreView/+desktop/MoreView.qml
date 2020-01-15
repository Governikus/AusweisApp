/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQml.Models 2.3

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.TitleBar 1.0
import Governikus.FeedbackView 1.0
import Governikus.InformationView 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: sectionPage

	enum SubViews {
		None,
		Diagnosis,
		ApplicationLog
	}

	property int activeSubView

	//: LABEL DESKTOP_QML
	Accessible.name: qsTr("Help section") + SettingsModel.translationTrigger
	//: LABEL DESKTOP_QML
	Accessible.description: qsTr("This is the help section of the AusweisApp2.") + SettingsModel.translationTrigger
	Keys.onEscapePressed: {
		if (activeSubView === MoreView.SubViews.None) {
			event.accepted = false
			return
		}

		activeSubView = MoreView.SubViews.None
	}

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Help") + SettingsModel.translationTrigger
		helpTopic: Utils.helpTopicOf(tabbedPane.currentContentItem, "helpSection")
		onClicked: activeSubView = MoreView.SubViews.None
	}

	TabbedPane {
		id: tabbedPane

		visible: activeSubView === MoreView.SubViews.None
		anchors.fill: parent
		anchors.margins: Constants.pane_spacing

		sectionsModel: [
			//: LABEL DESKTOP_QML
			qsTr("General") + SettingsModel.translationTrigger,
			//: LABEL DESKTOP_QML
			qsTr("Diagnosis and log") + SettingsModel.translationTrigger,
			//: LABEL DESKTOP_QML
			qsTr("Version information") + SettingsModel.translationTrigger
		]

		sectionDelegate: TabbedPaneDelegateOneLineText {
			sectionName: model ? model.modelData : ""
		}

		contentObjectModel: ObjectModel {
			Component { MoreViewGeneral {} }
			Component { MoreViewDiagnosis {} }
			Component { VersionInformation {} }
		}
	}

	Component {
		id: diagnosisView

		DiagnosisView {}
	}

	Component {
		id: versionInformation

		VersionInformation {}
	}

	Component {
		id: logFileView

		LogView {}
	}

	Loader {
		readonly property bool sectionPageTypeMarker: true
		property var titleBarAction: item ? item.titleBarAction : undefined

		visible: item // Otherwise onVisibleChildrenChanged and onVisibleChanged won't be triggered
		anchors.fill: parent

		sourceComponent: switch(sectionPage.activeSubView) {
			case MoreView.SubViews.Diagnosis: return diagnosisView
			case MoreView.SubViews.ApplicationLog: return logFileView
		}
	}
}
