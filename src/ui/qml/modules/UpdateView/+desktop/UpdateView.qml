/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.View

FlickableSectionPage {
	id: root

	readonly property bool downloadRunning: SettingsModel.appUpdateData.downloadRunning
	readonly property var updateData: SettingsModel.appUpdateData

	spacing: Style.dimens.pane_spacing

	//: DESKTOP
	title: qsTr("Application update")

	titleBarSettings: TitleBarSettings {
		navigationAction: root.downloadRunning ? NavigationAction.Action.Cancel : NavigationAction.Action.Back

		onNavigationActionClicked: {
			if (root.downloadRunning) {
				root.updateData.abortDownload();
			}
			root.leaveView();
		}
	}

	Subheading {
		elide: Text.ElideRight
		maximumLineCount: 1
		//: DESKTOP
		text: qsTr("Update available")
	}
	GText {
		//: DESKTOP %1 is replaced with the current version number
		text: qsTr("An update for the outdated installed version (%1) is available for download.").arg(Qt.application.version)
	}
	UpdateViewInformation {
		id: updateInformation

		Layout.fillWidth: true
		downloadSize: root.updateData.size
		releaseDate: root.updateData.date
		version: root.updateData.version
	}
	UpdateViewButtonRow {
		id: updateButtons

		Layout.fillWidth: true
		downloadInProgress: root.downloadRunning
		downloadProgressKB: root.updateData.downloadProgress
		downloadTotalKB: root.updateData.downloadTotal
		version: root.updateData.version

		onAbortDownload: root.updateData.abortDownload()
		onStartDownload: download.exec()
	}
	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		contentSpacing: 0
		shadowScale: 1.005

		ReleaseInformationModel {
			id: releaseInformationModel

		}
		Repeater {
			id: releaseInfoRepeater

			model: releaseInformationModel.updateRelease

			FormattedTextView {
				Layout.fillWidth: true
				color: Style.color.transparent
				count: releaseInfoRepeater.count

				onActiveFocusChanged: if (activeFocus) {
					Utils.positionViewAtItem(this);
				}

				FocusFrame {
				}
			}
		}
	}
	Connections {
		function onFireAppDownloadFinished() {
			UiPluginModel.fireQuitApplicationRequest();
		}
		function onFireAppUpdateFailed(pError, pSupportInfo) {
			warning.exec(pError, pSupportInfo);
		}

		target: root.updateData
	}
	ConfirmationPopup {
		id: download

		function exec() {
			if (root.updateData.compatible)
				load();
			else
				open();
		}
		function load() {
			root.updateData.download();
		}

		closePolicy: Popup.NoAutoClose
		//: DESKTOP
		okButtonText: qsTr("Update anyway")
		//: DESKTOP Text of the popup that is shown when the requested update is not compatible with the OS.
		text: qsTr("Troubleshooting and user support for the %1 is no longer available for your current operating system. Version %2 has not been tested with your current operating system, use is at your own risk.").arg(Qt.application.name).arg(root.updateData.version)
		//: DESKTOP Header of the popup that is shown when the requested update is not compatible with the OS.
		title: qsTr("Warning - Your operating system is no longer supported")

		onCancelled: close()
		onConfirmed: {
			load();
			close();
		}
	}
	ConfirmationPopup {
		id: warning

		function exec(pError, pSupportInfo) {
			text = pError;
			supportInfoText.text = pSupportInfo;
			open();
		}

		style: ConfirmationPopup.PopupStyle.OkButton
		//: DESKTOP Header of the popup that is shown when the app download failed.
		title: qsTr("Warning - Update failed")

		GText {
			id: supportInfoText

			visible: text !== ""
			width: parent.width
		}
	}
}
