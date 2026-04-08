/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Animations
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

	states: [
		State {
			name: "appcastRunning"
			when: SettingsModel.appUpdateData.appcastRunning

			PropertyChanges {
				updateInfoContent.visible: false
				updateLoadingContent.visible: true
			}
		},
		State {
			name: "appcastFailed"
			when: SettingsModel.appUpdateData.appcastFailed

			PropertyChanges {
				updateAvailableInfoContent.visible: false
				updateFailedContent.visible: true
				updateInfoContent.visible: false
			}
		}
	]
	titleBarSettings: TitleBarSettings {
		navigationAction: root.downloadRunning ? NavigationAction.Action.Cancel : NavigationAction.Action.Back

		onNavigationActionClicked: {
			if (root.downloadRunning) {
				root.updateData.abortDownload();
			}
			root.leaveView();
		}
	}
	transitions: [
		Transition {
			from: "appcastRunning"
			to: ""

			ScriptAction {
				script: {
					let version = SettingsModel.appUpdateData.version;
					if (version === "" || d.updateVersionBeforeRefresh === "")
						return;

					if (d.updateVersionBeforeRefresh !== version)
						newVersionConfirmation.exec();
				}
			}
		}
	]

	Component.onCompleted: d.refreshUpdateInformation()

	QtObject {
		id: d

		property string updateVersionBeforeRefresh: ""

		function refreshUpdateInformation() {
			if (SettingsModel.appUpdateData.appcastRunning)
				return;

			let version = SettingsModel.appUpdateData.version;
			if (version !== "")
				d.updateVersionBeforeRefresh = version;
			SettingsModel.updateAppcast();
		}
	}
	ColumnLayout {
		id: updateAvailableInfoContent

		spacing: root.spacing

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
	}
	RowLayout {
		id: updateLoadingContent

		spacing: Style.dimens.pane_spacing
		visible: false

		SpinningLoader {
		}
		GText {
			//: DESKTOP
			text: qsTr("Update information is being checked...")
		}
	}
	ColumnLayout {
		id: updateInfoContent

		spacing: root.spacing

		UpdateViewInformation {
			Layout.fillWidth: true
			downloadSize: root.updateData.size
			releaseDate: root.updateData.date
			version: root.updateData.version
		}
		UpdateViewButtonRow {
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

				FormattedTextPaneDelegate {
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
	}
	ColumnLayout {
		id: updateFailedContent

		Layout.preferredWidth: Style.dimens.max_text_width
		visible: false

		Heading {
			//: DESKTOP
			text: qsTr("Update failed")
			wrapMode: Text.WordWrap
		}
		AnimationLoader {
			Layout.alignment: Qt.AlignHCenter
			Layout.bottomMargin: Style.dimens.pane_spacing
			animated: false
			symbol: Symbol.Type.ERROR
			type: AnimationLoader.Type.STATUS
		}
		Subheading {
			Layout.topMargin: Style.dimens.pane_spacing

			//: DESKTOP
			text: qsTr("Application update could not be performed")
		}
		GText {
			Layout.topMargin: Style.dimens.text_spacing
			text: SettingsModel.appUpdateData.appcastErrorText
		}
		ButtonBox {
			id: buttonBox

			SecondaryButton {
				Layout.maximumWidth: buttonBox.uniformButtonWidth
				Layout.preferredWidth: buttonBox.uniformButtonWidth

				//: DESKTOP
				text: qsTr("Back to start page")

				onClicked: root.popAll()
			}
			GButton {
				Layout.maximumWidth: buttonBox.uniformButtonWidth
				Layout.preferredWidth: buttonBox.uniformButtonWidth

				//: DESKTOP
				text: qsTr("Retry")

				onClicked: d.refreshUpdateInformation()
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
		id: newVersionConfirmation

		function exec() {
			open();
		}

		//: DESKTOP
		cancelButtonText: qsTr("Back to start page")
		closePolicy: Popup.NoAutoClose
		//: DESKTOP
		okButtonText: qsTr("Show update")
		//: DESKTOP %1 will be replaced with the update version number
		text: qsTr("A newer update (version %1) has been found and is available for download.").arg(root.updateData.version)
		//: DESKTOP %1 will be replaced with the update version number
		title: qsTr("Newer version %1 available").arg(root.updateData.version)

		onCancelled: root.popAll()
		onConfirmed: close()
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
