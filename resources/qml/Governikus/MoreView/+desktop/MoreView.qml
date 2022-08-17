/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.TitleBar 1.0
import Governikus.FeedbackView 1.0
import Governikus.InformationView 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	id: sectionPage

	enum SubViews {
		None,
		Diagnosis,
		ApplicationLog
	}

	property int activeSubView

	Keys.onEscapePressed: {
		if (activeSubView === MoreView.SubViews.None) {
			event.accepted = false
			return
		}

		activeSubView = MoreView.SubViews.None
	}

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("Help")
		helpTopic: Utils.helpTopicOf(tabbedPane.currentContentItem, "helpSection")
		onClicked: activeSubView = MoreView.SubViews.None
	}

	TabbedPane {
		id: tabbedPane

		visible: activeSubView === MoreView.SubViews.None
		anchors.fill: parent
		anchors.margins: Constants.pane_spacing

		sectionsModel: [
			//: LABEL DESKTOP
			qsTr("General"),
			//: LABEL DESKTOP
			qsTr("Diagnosis and logs"),
			//: LABEL DESKTOP
			qsTr("Version information"),
			//: LABEL DESKTOP
			qsTr("Software license"),
			//: LABEL DESKTOP
			qsTr("Release notes")
		]

		sectionDelegate: TabbedPaneDelegateOneLineText {
			sectionName: model ? model.modelData : ""
		}

		contentObjectModel: ObjectModel {
			Component { MoreViewGeneral {} }
			Component { MoreViewDiagnosis {} }
			Component { VersionInformation {} }
			Component { LicenseInformation {
				height: tabbedPane.availableHeight
				anchors {
					left: parent.left
					right: parent.right
					rightMargin: -Constants.pane_padding
				}
			} }
			Component { ReleaseNotes {
				height: tabbedPane.availableHeight
				anchors {
					left: parent.left
					right: parent.right
					rightMargin: -Constants.pane_padding
				}
			} }
		}
	}

	Component {
		id: diagnosisView

		DiagnosisView {}
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
