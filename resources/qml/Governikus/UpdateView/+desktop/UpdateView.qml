/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.ResultView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0


SectionPage {
	id: root

	signal leaveView()

	Accessible.name: qsTr("Application update view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the application update panel of the AusweisApp2.") + SettingsModel.translationTrigger

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Application update") + SettingsModel.translationTrigger
		showHelp: false
	}

	ResultView {
		visible: !SettingsModel.appUpdateData.valid

		buttonType: NavigationButton.Type.Back
		resultType: ResultView.Type.IsError
		//: LABEL DESKTOP_QML Placext if the update information are invalid, might be caused by network issues.
		text: qsTr("The update information could not be retrieved. Please check your network connection.") + SettingsModel.translationTrigger

		onNextView: leaveView()
	}

	ResultView {
		visible: !SettingsModel.appUpdateAvailable && SettingsModel.appUpdateData.valid

		buttonType: NavigationButton.Type.Back
		resultType: ResultView.Type.IsSuccess
		//: LABEL DESKTOP_QML The currently installed version is the most recent one, no action is required.
		text: qsTr("Your version %1 of %2 is up to date!").arg(Qt.application.version).arg(Qt.application.name) + SettingsModel.translationTrigger

		onNextView: leaveView()
	}

	Pane {
		id: pane

		visible: SettingsModel.appUpdateData.valid && SettingsModel.appUpdateData.updateAvailable
		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		activeFocusOnTab: true

		title: qsTr("An update is available (installed version %1)").arg(Qt.application.version) + SettingsModel.translationTrigger

		ColumnLayout {
			width: parent.width
			height: pane.availableContentHeight

			UpdateViewInformation {
				id: updateInformation

				Layout.fillWidth: true

				releaseDate: SettingsModel.appUpdateData.date
				version: SettingsModel.appUpdateData.version
				downloadSize: SettingsModel.appUpdateData.size
			}

			GSeparator {
				Layout.fillWidth: true
				Layout.topMargin: Constants.text_spacing
				Layout.bottomMargin: Layout.topMargin
			}

			UpdateViewReleaseNotes {
				id: releaseNotesView

				Layout.fillWidth: true
				Layout.fillHeight: true

				releaseNotes: SettingsModel.appUpdateData.notes
			}

			GSeparator {
				Layout.fillWidth: true
				Layout.topMargin: Constants.text_spacing
				Layout.bottomMargin: Layout.topMargin
			}

			UpdateViewButtonRow {
				id: updateButtons

				Layout.fillWidth: true

				downloadTooltip: SettingsModel.appUpdateData.url

				onDownloadUpdate: {
					Qt.openUrlExternally(SettingsModel.appUpdateData.url)
					root.leaveView()
				}
				onSkipUpdate: {
					SettingsModel.appUpdateData.skipUpdate()
					root.leaveView()
				}
				onRemindLater: root.leaveView()
			}
		}
	}

	Keys.onPressed: {
		if (event.key === Qt.Key_PageDown)
			releaseNotesView.scrollDown()
		else if (event.key === Qt.Key_PageUp)
			releaseNotesView.scrollUp()
	}
}
