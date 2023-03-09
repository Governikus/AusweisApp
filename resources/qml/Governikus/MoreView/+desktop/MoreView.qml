/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
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

	titleBarAction: TitleBarAction {
		helpTopic: Utils.helpTopicOf(tabbedPane.currentContentItem, "helpSection")
		//: LABEL DESKTOP
		text: qsTr("Help")

		onClicked: activeSubView = MoreView.SubViews.None
	}

	Keys.onEscapePressed: event => {
		if (activeSubView === MoreView.SubViews.None) {
			event.accepted = false;
			return;
		}
		activeSubView = MoreView.SubViews.None;
	}

	TabbedPane {
		id: tabbedPane
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
			qsTr("Release notes")]
		visible: activeSubView === MoreView.SubViews.None

		contentObjectModel: ObjectModel {
			Component {
				MoreViewGeneral {
				}
			}
			Component {
				MoreViewDiagnosis {
				}
			}
			Component {
				VersionInformation {
				}
			}
			Component {
				LicenseInformation {
					height: tabbedPane.availableHeight

					anchors {
						left: parent.left
						right: parent.right
						rightMargin: -Constants.pane_padding
					}
				}
			}
			Component {
				ReleaseNotes {
					height: tabbedPane.availableHeight

					anchors {
						left: parent.left
						right: parent.right
						rightMargin: -Constants.pane_padding
					}
				}
			}
		}
		sectionDelegate: TabbedPaneDelegateText {
			sectionName: model ? model.modelData : ""
		}
	}
	Component {
		id: diagnosisView
		DiagnosisView {
		}
	}
	Component {
		id: logFileView
		LogView {
		}
	}
	Loader {
		readonly property bool sectionPageTypeMarker: true
		property var titleBarAction: item ? item.titleBarAction : undefined

		anchors.fill: parent
		sourceComponent: switch (sectionPage.activeSubView) {
		case MoreView.SubViews.Diagnosis:
			return diagnosisView;
		case MoreView.SubViews.ApplicationLog:
			return logFileView;
		}
		visible: item // Otherwise onVisibleChildrenChanged and onVisibleChanged won't be triggered
	}
}
