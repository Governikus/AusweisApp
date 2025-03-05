/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQml.Models

import Governikus.Global
import Governikus.View
import Governikus.TitleBar
import Governikus.FeedbackView
import Governikus.InformationView
import Governikus.Style
import Governikus.Type

SectionPage {
	id: root

	signal requestUiModule(var pUiModule)

	//: LABEL DESKTOP
	title: qsTr("Help")

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Back

		onNavigationActionClicked: root.pop()
	}

	Keys.onPressed: event => {
		tabbedPane.handleKeyPress(event.key);
	}

	TabbedPane {
		id: tabbedPane

		anchors.fill: parent
		contentRightMargin: currentContentItem instanceof LicenseInformation || currentContentItem instanceof ReleaseNotes ? 0 : Style.dimens.pane_padding
		sectionsModel: [
			//: LABEL DESKTOP
			qsTr("General"),
			//: LABEL DESKTOP
			qsTr("Data and logs"),
			//: LABEL DESKTOP
			qsTr("Information"),
			//: LABEL DESKTOP
			qsTr("Software license"),
			//: LABEL DESKTOP
			qsTr("Release notes")]

		contentObjectModel: ObjectModel {
			Component {
				MoreViewGeneral {
					onStartOnboarding: root.requestUiModule(UiModule.ONBOARDING)
				}
			}
			Component {
				MoreViewDiagnosis {
					onShowDiagnosis: root.push(diagnosisView)
					onShowLogs: root.push(logFileView)

					Component {
						id: diagnosisView

						DiagnosisView {
							onLeaveView: pop()
						}
					}
					Component {
						id: logFileView

						LogView {
							onLeaveView: pop()
						}
					}
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
}
