/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.ResultView
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.View

FlickableSectionPage {
	id: root

	property alias downloadRunning: updateButtons.downloadInProgress
	readonly property var update: SettingsModel.appUpdateData

	fillWidth: true
	//: LABEL DESKTOP
	title: qsTr("Application update")

	titleBarSettings: TitleBarSettings {
		navigationAction: root.downloadRunning ? NavigationAction.Cancel : NavigationAction.Back

		onNavigationActionClicked: {
			if (root.downloadRunning) {
				root.update.abortDownload();
			}
			root.leaveView();
		}
	}

	ResultView {
		Layout.fillHeight: true
		Layout.fillWidth: true
		anchors.fill: null
		animationSymbol: Symbol.Type.ERROR
		animationType: root.update.missingPlatform ? AnimationLoader.Type.STATUS : AnimationLoader.Type.NETWORK_ERROR
		showOkButton: false
		text: root.update.missingPlatform ?
		//: LABEL DESKTOP Resulttext if no update information is available for the current platform.
		qsTr("An update information for your platform is not available.") :
		//: LABEL DESKTOP Resulttext if the update information are invalid, might be caused by network issues.
		qsTr("The update information could not be retrieved. Please check your network connection.")
		title: root.title
		visible: !root.update.valid

		onLeaveView: root.leaveView()
	}
	ResultView {
		Layout.fillHeight: true
		Layout.fillWidth: true
		anchors.fill: null
		animationSymbol: Symbol.Type.CHECK
		animationType: AnimationLoader.Type.STATUS
		showOkButton: false
		//: LABEL DESKTOP The currently installed version is the most recent one, no action is required.
		text: qsTr("Your version %1 of %2 is up to date!").arg(Qt.application.version).arg(Qt.application.name)
		title: root.title
		visible: root.update.valid && !root.update.updateAvailable

		onLeaveView: root.leaveView()
	}
	ColumnLayout {
		spacing: Style.dimens.pane_spacing
		visible: root.update.valid && root.update.updateAvailable

		GText {
			Layout.leftMargin: Style.dimens.pane_padding
			Layout.rightMargin: Layout.leftMargin
			elide: Text.ElideRight
			maximumLineCount: 1
			text: qsTr("An update is available (installed version %1)").arg(Qt.application.version)
			textStyle: Style.text.subline
		}
		UpdateViewInformation {
			id: updateInformation

			Layout.fillWidth: true
			Layout.leftMargin: Style.dimens.pane_padding
			Layout.rightMargin: Layout.leftMargin
			downloadSize: root.update.size
			releaseDate: root.update.date
			version: root.update.version
		}
		GSeparator {
			Layout.fillWidth: true
		}
		UpdateViewButtonRow {
			id: updateButtons

			Layout.fillWidth: true
			Layout.leftMargin: Style.dimens.pane_padding
			Layout.rightMargin: Layout.leftMargin
			progressText: "%1 KiB / %2 KiB".arg(root.update.downloadProgress.toLocaleString(Qt.locale(SettingsModel.language), "f", 0)).arg(root.update.downloadTotal.toLocaleString(Qt.locale(SettingsModel.language), "f", 0))
			progressValue: root.update.downloadProgress * 100 / root.update.downloadTotal

			onToggleUpdate: root.downloadRunning ? root.update.abortDownload() : download.exec()
		}
		GSeparator {
			Layout.fillWidth: true
		}
		GPane {
			Layout.fillWidth: true
			contentPadding: 0
			shadowScale: 1.005
			spacing: 0

			ReleaseInformationModel {
				id: releaseInformationModel

			}
			Repeater {
				id: releaseInfoRepeater

				model: releaseInformationModel.updateRelease

				FormattedTextView {
					Layout.fillWidth: true
					color: Style.color.transparent
					totalItemCount: releaseInfoRepeater.count
				}
			}
		}
	}
	Connections {
		function onFireAppDownloadFinished() {
			UiPluginModel.fireQuitApplicationRequest();
		}
		function onFireAppUpdateAborted() {
			root.downloadRunning = false;
		}
		function onFireAppUpdateFailed(pError, pSupportInfo) {
			warning.exec(pError, pSupportInfo);
		}

		target: root.update
	}
	ConfirmationPopup {
		id: download

		function exec() {
			if (root.update.compatible)
				load();
			else
				open();
		}
		function load() {
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

		function exec(pError, pSupportInfo) {
			root.downloadRunning = false;
			text = pError;
			supportInfoText.text = pSupportInfo;
			open();
		}

		style: ConfirmationPopup.PopupStyle.OkButton
		//: INFO DESKTOP Header of the popup that is shown when the app download failed.
		title: qsTr("Warning - Update failed")

		GText {
			id: supportInfoText

			visible: text !== ""
			width: parent.width
		}
	}
}
