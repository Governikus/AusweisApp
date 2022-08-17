/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.InformationView 1.0
import Governikus.ResultView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.GlobalStatus 1.0
import Governikus.Type.ReleaseInformationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0


SectionPage {
	id: root

	property alias downloadRunning: updateButtons.downloadInProgress
	readonly property var update: SettingsModel.appUpdateData
	signal leaveView()

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("Application update")
		helpTopic: "applicationUpdate"
	}

	ResultView {
		visible: !root.update.valid

		buttonType: NavigationButton.Type.Back
		resultType: ResultView.Type.IsError
		text: root.update.missingPlatform
			  //: LABEL DESKTOP Resulttext if no update information is available for the current platform.
			  ? qsTr("An update information for your platform is not available.")
			  //: LABEL DESKTOP Resulttext if the update information are invalid, might be caused by network issues.
			  : qsTr("The update information could not be retrieved. Please check your network connection.")

		onNextView: leaveView()
	}

	ResultView {
		visible: !SettingsModel.appUpdateAvailable && root.update.valid

		buttonType: NavigationButton.Type.Back
		resultType: ResultView.Type.IsSuccess
		//: LABEL DESKTOP The currently installed version is the most recent one, no action is required.
		text: qsTr("Your version %1 of %2 is up to date!").arg(Qt.application.version).arg(Qt.application.name)

		onNextView: leaveView()
	}

	GPane {
		id: pane

		visible: root.update.valid && root.update.updateAvailable

		anchors {
			fill: parent
			margins: Constants.pane_padding
		}

		activeFocusOnTab: true

		title: qsTr("An update is available (installed version %1)").arg(Qt.application.version)

		ColumnLayout {
			width: parent.width
			height: pane.availableContentHeight

			UpdateViewInformation {
				id: updateInformation

				Layout.fillWidth: true

				releaseDate: root.update.date
				version: root.update.version
				downloadSize: root.update.size
			}

			GSeparator {
				Layout.fillWidth: true
				Layout.topMargin: Constants.text_spacing
				Layout.bottomMargin: Layout.topMargin
			}

			Item {
				Layout.fillWidth: true
				Layout.fillHeight: true

				clip: true

				ReleaseNotesView {
					anchors {
						fill: parent
						leftMargin: Constants.pane_padding
						topMargin: Constants.pane_padding
						bottomMargin: Constants.pane_padding
					}

					model: releaseInformationModel.updateRelease

					ReleaseInformationModel {
						id: releaseInformationModel
					}

				}

				ScrollGradients {
					anchors.fill: parent

					leftMargin: 0
					rightMargin: 0
					color: Style.color.background_pane
				}
			}

			GSeparator {
				Layout.fillWidth: true
				Layout.topMargin: Constants.text_spacing
				Layout.bottomMargin: Layout.topMargin
			}

			UpdateViewButtonRow {
				id: updateButtons

				Layout.fillWidth: true

				progressText: "%1 KiB / %2 KiB"
							  .arg(root.update.downloadProgress.toLocaleString(Qt.locale(SettingsModel.language), "f", 0))
							  .arg(root.update.downloadTotal.toLocaleString(Qt.locale(SettingsModel.language), "f", 0))
				progressValue: root.update.downloadProgress * 100 / root.update.downloadTotal
				onToggleUpdate: root.downloadRunning ? root.update.abortDownload() : download.exec()
				onSkipUpdate: {
					root.update.skipUpdate()
					root.leaveView()
				}
				onRemindLater: root.leaveView()
			}
		}
	}

	Connections {
		target: root.update
		function onFireAppUpdateFailed(pError) { warning.exec(pError) }
		function onFireAppDownloadFinished() { plugin.fireQuitApplicationRequest() }
	}

	ConfirmationPopup {
		id: download

		function exec() {
			if (root.update.compatible || Qt.platform.os === "osx")
				load()
			else
				open()
		}

		function load() {
			if (Qt.platform.os === "osx") {
				Qt.openUrlExternally(ApplicationModel.storeUrl)
				return
			}

			root.downloadRunning = true
			root.update.download()
		}

		closePolicy: Popup.NoAutoClose
		//: INFO DESKTOP Header of the popup that is shown when the requested update is not compatible with the OS.
		title: qsTr("Warning - Your operating system is no longer supported")
		//: INFO DESKTOP Text of the popup that is shown when the requested update is not compatible with the OS.
		text: qsTr("Troubleshooting and user support for the %1 is no longer available for your current operating system. Version %2 has not been tested with your current operating system, use is at your own risk.").arg(Qt.application.name).arg(root.update.version)
		//: LABEL DESKTOP
		okButtonText: qsTr("Update anyway")

		onConfirmed: {
			load()
			close()
		}
		onCancelled: close()
	}

	ConfirmationPopup {
		id: warning

		property int error: GlobalStatus.No_Error

		function exec(pError) {
			root.downloadRunning = false
			if (pError !== GlobalStatus.Downloader_Aborted) {
				error = pError
				open()
			}
		}

		style: ConfirmationPopup.PopupStyle.OkButton
		//: INFO DESKTOP Header of the popup that is shown when the app download failed.
		title: qsTr("Warning - Update failed")
		text: switch (error) {
			case GlobalStatus.Downloader_Data_Corrupted:
				//: INFO DESKTOP Text of the popup that is shown when the app download failed because of a wrong checksum.
				return qsTr("The received data is broken. Check your network connection and try to restart the update.")
			case GlobalStatus.Update_Execution_Failed:
				//: INFO DESKTOP Text of the popup that is shown when the execution of the update failed (1/2).
				return qsTr("The update could not be started automatically after a successful download. Please try to do a manual update. You can find the downloaded file %1here%2.").arg(
								"<a href=\"%1\">".arg(SettingsModel.appUpdateData.downloadFolder)
							).arg("</a>")
			default:
				//: INFO DESKTOP Generic text of the popup that is shown when the app download failed.
				return qsTr("An unknown network error occurred. Check your network connection and try to restart the update.")
		}

		GText {
			visible: warning.error === GlobalStatus.Update_Execution_Failed
			width: parent.width

			activeFocusOnTab: true

			//: INFO DESKTOP Text of the popup that is shown when the execution of the update failed (2/2).
			text: qsTr("If this does not help, contact our %1support%2.").arg(
					  "<a href=\"%1\">".arg("https://www.ausweisapp.bund.de/%1/aa2/support".arg(SettingsModel.language))
				  ).arg("</a>")

			FocusFrame {}
		}
	}
}
