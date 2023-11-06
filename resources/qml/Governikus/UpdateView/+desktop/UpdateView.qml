/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.InformationView
import Governikus.ResultView
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type.ApplicationModel
import Governikus.Type.GlobalStatus
import Governikus.Type.ReleaseInformationModel
import Governikus.Type.SettingsModel
import Governikus.View

SectionPage {
	id: root

	property alias downloadRunning: updateButtons.downloadInProgress
	readonly property var update: SettingsModel.appUpdateData

	signal leaveView

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("Application update")
	}

	ResultView {
		buttonType: NavigationButton.Type.Back
		icon: root.update.missingPlatform ? "qrc:///images/status_error_%1.svg".arg(Style.currentTheme.name) : "qrc:///images/workflow_error_network_%1.svg".arg(Style.currentTheme.name)
		text: root.update.missingPlatform ?
		//: LABEL DESKTOP Resulttext if no update information is available for the current platform.
		qsTr("An update information for your platform is not available.") :
		//: LABEL DESKTOP Resulttext if the update information are invalid, might be caused by network issues.
		qsTr("The update information could not be retrieved. Please check your network connection.")
		visible: !root.update.valid

		onNextView: leaveView()
	}
	ResultView {
		buttonType: NavigationButton.Type.Back
		icon: "qrc:///images/status_ok_%1.svg".arg(Style.currentTheme.name)
		//: LABEL DESKTOP The currently installed version is the most recent one, no action is required.
		text: qsTr("Your version %1 of %2 is up to date!").arg(Qt.application.version).arg(Qt.application.name)
		visible: !SettingsModel.appUpdateAvailable && root.update.valid

		onNextView: leaveView()
	}
	GPane {
		id: pane

		activeFocusOnTab: true
		title: qsTr("An update is available (installed version %1)").arg(Qt.application.version)
		visible: root.update.valid && root.update.updateAvailable

		anchors {
			fill: parent
			margins: Constants.pane_padding
		}
		UpdateViewInformation {
			id: updateInformation

			Layout.fillWidth: true
			downloadSize: root.update.size
			releaseDate: root.update.date
			version: root.update.version
		}
		GSeparator {
			Layout.bottomMargin: Layout.topMargin
			Layout.fillWidth: true
			Layout.topMargin: Constants.text_spacing
		}
		Item {
			Layout.fillHeight: true
			Layout.fillWidth: true
			clip: true

			ReleaseNotesView {
				model: releaseInformationModel.updateRelease

				anchors {
					bottomMargin: Constants.pane_padding
					fill: parent
					leftMargin: Constants.pane_padding
					topMargin: Constants.pane_padding
				}
				ReleaseInformationModel {
					id: releaseInformationModel

				}
			}
			ScrollGradients {
				anchors.fill: parent
				color: Style.color.pane
				leftMargin: 0
				rightMargin: 0
			}
		}
		GSeparator {
			Layout.bottomMargin: Layout.topMargin
			Layout.fillWidth: true
			Layout.topMargin: Constants.text_spacing
		}
		UpdateViewButtonRow {
			id: updateButtons

			Layout.fillWidth: true
			progressText: "%1 KiB / %2 KiB".arg(root.update.downloadProgress.toLocaleString(Qt.locale(SettingsModel.language), "f", 0)).arg(root.update.downloadTotal.toLocaleString(Qt.locale(SettingsModel.language), "f", 0))
			progressValue: root.update.downloadProgress * 100 / root.update.downloadTotal

			onRemindLater: root.leaveView()
			onSkipUpdate: {
				root.update.skipUpdate();
				root.leaveView();
			}
			onToggleUpdate: root.downloadRunning ? root.update.abortDownload() : download.exec()
		}
	}
	Connections {
		function onFireAppDownloadFinished() {
			plugin.fireQuitApplicationRequest();
		}
		function onFireAppUpdateFailed(pError) {
			warning.exec(pError);
		}

		target: root.update
	}
	ConfirmationPopup {
		id: download

		function exec() {
			if (root.update.compatible || Qt.platform.os === "osx")
				load();
			else
				open();
		}
		function load() {
			if (Qt.platform.os === "osx") {
				Qt.openUrlExternally(ApplicationModel.storeUrl);
				return;
			}
			root.downloadRunning = true;
			root.update.download();
		}

		closePolicy: Popup.NoAutoClose
		//: LABEL DESKTOP
		okButtonText: qsTr("Update anyway")
		//: INFO DESKTOP Text of the popup that is shown when the requested update is not compatible with the OS.
		text: qsTr("Troubleshooting and user support for the %1 is no longer available for your current operating system. Version %2 has not been tested with your current operating system, use is at your own risk.").arg(Qt.application.name).arg(root.update.version)
		//: INFO DESKTOP Header of the popup that is shown when the requested update is not compatible with the OS.
		title: qsTr("Warning - Your operating system is no longer supported")

		onCancelled: close()
		onConfirmed: {
			load();
			close();
		}
	}
	ConfirmationPopup {
		id: warning

		property int error: GlobalStatus.No_Error

		function exec(pError) {
			root.downloadRunning = false;
			if (pError !== GlobalStatus.Downloader_Aborted) {
				error = pError;
				open();
			}
		}

		style: ConfirmationPopup.PopupStyle.OkButton
		text: switch (error) {
		case GlobalStatus.Downloader_Data_Corrupted:
			//: INFO DESKTOP Text of the popup that is shown when the app download failed because of a wrong checksum.
			return qsTr("The received data is broken. Check your network connection and try to restart the update.");
		case GlobalStatus.Update_Execution_Failed:
			//: INFO DESKTOP Text of the popup that is shown when the execution of the update failed (1/2).
			return qsTr("The update could not be started automatically after a successful download. Please try to do a manual update. You can find the downloaded file %1here%2.").arg("<a href=\"%1\">".arg(SettingsModel.appUpdateData.downloadFolder)).arg("</a>");
		default:
			//: INFO DESKTOP Generic text of the popup that is shown when the app download failed.
			return qsTr("An unknown network error occurred. Check your network connection and try to restart the update.");
		}
		//: INFO DESKTOP Header of the popup that is shown when the app download failed.
		title: qsTr("Warning - Update failed")

		GText {
			activeFocusOnTab: true

			//: INFO DESKTOP Text of the popup that is shown when the execution of the update failed (2/2).
			text: qsTr("If this does not help, contact our %1support%2.").arg("<a href=\"%1\">".arg("https://www.ausweisapp.bund.de/%1/aa2/support".arg(SettingsModel.language))).arg("</a>")
			visible: warning.error === GlobalStatus.Update_Execution_Failed
			width: parent.width

			FocusFrame {
			}
		}
	}
}
