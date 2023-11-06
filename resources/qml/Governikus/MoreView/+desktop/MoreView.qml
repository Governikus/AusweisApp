/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQml.Models
import Governikus.Global
import Governikus.View
import Governikus.TitleBar
import Governikus.FeedbackView
import Governikus.InformationView
import Governikus.Type.ApplicationModel

SectionPage {
	id: sectionPage

	enum SubViews {
		None,
		Diagnosis,
		ApplicationLog
	}

	property int activeSubView

	titleBarAction: TitleBarAction {
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
		contentRightMargin: currentContentItem instanceof LicenseInformation || currentContentItem instanceof ReleaseNotes ? 0 : Constants.pane_padding
		sectionsModel: [
			//: LABEL DESKTOP
			qsTr("General"),
			//: LABEL DESKTOP
			qsTr("Data and logs"),
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
					onShowDiagnosis: sectionPage.activeSubView = MoreView.SubViews.Diagnosis
					onShowLogs: sectionPage.activeSubView = MoreView.SubViews.ApplicationLog
				}
			}
			Component {
				VersionInformation {
				}
			}
			Component {
				LicenseInformation {
					Layout.preferredHeight: tabbedPane.availableHeight
				}
			}
			Component {
				ReleaseNotes {
					Layout.preferredHeight: tabbedPane.availableHeight
				}
			}
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
		readonly property bool breadcrumpSearchPath: true
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
