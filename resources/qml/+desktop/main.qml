/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.MainView 1.0
import Governikus.HistoryView 1.0
import Governikus.SelfAuthenticationView 1.0
import Governikus.IdentifyView 1.0
import Governikus.ChangePinView 1.0
import Governikus.ProviderView 1.0
import Governikus.InformationView 1.0
import Governikus.SettingsView 1.0
import Governikus.TutorialView 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.UiModule 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Style 1.0

import QtQml 2.10
import QtQml.Models 2.10
import QtQuick 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.1


ApplicationWindow {
	id: appWindow

	function openSaveFileDialog(pOnAcceptedCallback, pDefaultFilename, pSuffix) {
		//: LABEL DESKTOP_QML
		saveFileDialog.nameFilters = [qsTr("Text files") + " (*." + pSuffix + ")"]
		saveFileDialog.defaultSuffix = pSuffix
		saveFileDialog.currentFile = saveFileDialog.folder + "/" + pDefaultFilename
		saveFileDialog.acceptedCallback = pOnAcceptedCallback
		saveFileDialog.open()
	}

	visible: false
	width: d.initialWidth
	height: d.initialHeight

	minimumWidth: 480
	minimumHeight: 360

	title: Qt.application.name
	color: Style.color.background
	menuBar: TitleBar {
		id: titleBar
		contentRoot: contentLoader
		onRootClicked: d.activeView = SectionPage.Views.Main
	}

	Component.onCompleted: menuBar.forceActiveFocus()

	onWidthChanged: d.setScaleFactor()
	onHeightChanged: d.setScaleFactor()

	onClosing: {
		if (SettingsModel.remindUserToClose) {
			closeWarning.open()
			close.accepted = false
		} else {
			hide()
			plugin.hide()
		}
	}
	onVisibilityChanged: if (visibility !== ApplicationWindow.Minimized) d.lastVisibility = visibility

	QtObject {
		id: d

		property int activeView: SectionPage.Views.Main
		property int lastVisibility: ApplicationWindow.Windowed
		readonly property int initialWidth: ApplicationModel.dpiScale * 1600
		readonly property int initialHeight: ApplicationModel.dpiScale * 1200

		function closeOpenDialogs() {
			saveFileDialog.reject()
			closeWarning.close()
		}

		function showMainWindow() {
			var currentFlags = flags
			// Force the window to the foreground if it was minimized (not closed to tray)
			if (Qt.platform.os === "windows") {
				flags = currentFlags | Qt.WindowStaysOnTopHint
			}

			if (d.lastVisibility === ApplicationWindow.Maximized) {
				showMaximized()
			} else {
				show()
			}

			flags = currentFlags
			raise()
			requestActivate()
		}

		function setScaleFactor() {
			ApplicationModel.scaleFactor = Math.min(width / initialWidth, height / initialHeight)
		}

	}

	FileDialog {
		id: saveFileDialog

		property var acceptedCallback

		fileMode: FileDialog.SaveFile
		folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
		onAccepted: acceptedCallback(file)
	}

	ConfirmationPopup {
		id: domination

		visible: plugin.dominated

		style: ConfirmationPopup.PopupStyle.NoButtons
		//: INFO DESKTOP_QML The AA2 is currently remote controlled via the SDK interface, concurrent usage of the AA2 is not possible.
		title: qsTr("Another application uses %1").arg(Qt.application.name)
		text: plugin.dominator
		closePolicy: Popup.NoAutoClose
	}

	ConfirmationPopup {
		id: closeWarning

		closePolicy: Popup.NoAutoClose
		style: ConfirmationPopup.PopupStyle.OkButton
		//: INFO DESKTOP_QML Header of the popup that is shown when the AA2 is closed for the first time.
		title: qsTr("The user interface of the %1 is closed.").arg(Qt.application.name) + SettingsModel.translationTrigger
		//: INFO DESKTOP_QML Content of the popup that is shown when the AA2 is closed for the first time.
		text: qsTr("The program remains available via the icon in the system tray. Click on the %1 icon to reopen the user interface.").arg(Qt.application.name) + SettingsModel.translationTrigger
		onConfirmed: {
			hide()
			plugin.hide()
		}

		ToggleableOption {
			//: LABEL DESKTOP_QML
			text: qsTr("Do not show this dialog again.") + SettingsModel.translationTrigger
			textStyle: Style.text.normal

			checked: !SettingsModel.remindUserToClose

			onCheckedChanged: SettingsModel.remindUserToClose = !checked
		}
	}

	Connections {
		target: plugin
		onFireShowRequest: {
			d.showMainWindow()
			d.closeOpenDialogs()
			switch (pModule) {
				case UiModule.IDENTIFY:
					if (ApplicationModel.currentWorkflow === "") {
						d.activeView = SectionPage.Views.SelfAuthentication
					}
					if (ApplicationModel.currentWorkflow === "authentication" || ApplicationModel.currentWorkflow === "selfauthentication") {
						d.activeView = SectionPage.Views.Identify
					}
					break
				case UiModule.PINMANAGEMENT:
					if (ApplicationModel.currentWorkflow === "") {
						ChangePinModel.startWorkflow()
					}
					if (ApplicationModel.currentWorkflow === "changepin") {
						d.activeView = SectionPage.Views.ChangePin
					}
					break
				case UiModule.DEFAULT:
					if (ApplicationModel.currentWorkflow === "") {
						d.activeView = SectionPage.Views.Main
					}
					break
				case UiModule.SETTINGS:
					if (ApplicationModel.currentWorkflow === "") {
						d.activeView = SectionPage.Views.Settings
					}
					break
				case UiModule.CURRENT:
					if (SettingsModel.showSetupAssistantOnStart) {
						d.activeView = SectionPage.Views.SetupAssistant
					}
					break
			}
		}
		onFireHideRequest: {
			hide()
		}
	}

	Connections {
		target: SettingsModel
		onFireAppUpdateDataChanged: {
			var updateData = SettingsModel.appUpdateData

			if (!updateData.valid) {
				//: INFO DESKTOP_QML Message that the update data is invalid and can't be used.
				ApplicationModel.showFeedback(qsTr("Unsupported version of %1.").arg(Qt.application.name))
			}
			else {
				if (pUpdateAvailable) {
					//: INFO DESKTOP_QML An update was found which matches the current platform, the new version number is shown in the message.
					ApplicationModel.showFeedback(qsTr("An update is available (Version: %1).").arg(updateData.version))
					//: INFO DESKTOP_QML An update was found. This is the caption of the download button, clicking it opens the link in the browser.
					ApplicationModel.showFeedback("<a href=\"%1\">".arg(updateData.url) + qsTr("Download") + "</a>")
					//: INFO DESKTOP_QML An update was found. This is the caption of the release note button, clicking it opens the note in the browser.
					ApplicationModel.showFeedback("<a href=\"%1\">".arg(updateData.notesUrl) + qsTr("Release notes") + "</a>")
				}
				else {
					//: INFO DESKTOP_QML The AA2 is up-to-date, this message is only shown if the update check is started by the user and not via the auto-update functionality.
					ApplicationModel.showFeedback(qsTr("Current version %1 is up to date.").arg(updateData.currentVersion))
				}
			}
		}
	}

	Action {
		shortcut: "Ctrl+Alt+R"
		onTriggered: plugin.developerBuild ? plugin.doRefresh() : ""
	}

	Shortcut {
		sequence: StandardKey.HelpContents
		onActivated: ApplicationModel.openOnlineHelp("index")
	}

	Loader {
		id: contentLoader

		// Keep in sync with the order/length of the SectionPage.Views enum
		property var sectionPages: ObjectModel {
			Component {MainView {}}
			Component {SelfAuthenticationView {}}
			Component {IdentifyView {}}
			Component {ChangePinView {}}
			Component {ProviderView {}}
			Component {InformationView {}}
			Component {SettingsView {}}
			Component {HistoryView {}}
			Component {SetupAssistantView {}}
		}

		Keys.onEscapePressed: {
			if (d.activeView !== SectionPage.Views.Main) {
				d.activeView = SectionPage.Views.Main
			}
		}

		Connections {
			target: contentLoader.item
			onNextView: d.activeView = pName
			onVisibleChildrenChanged: titleBar.updateActions()
		}

		anchors.fill: parent

		onItemChanged: titleBar.updateActions()
		sourceComponent: sectionPages.get(d.activeView)
	}

	Rectangle {
		visible: SettingsModel.developerMode && d.activeView !== SectionPage.Views.Settings
		height: childrenRect.height
		width: childrenRect.width
		anchors {
			bottom: parent.bottom
			bottomMargin: 4
			right: parent.right
			rightMargin: 4
		}

		color: Constants.white
		opacity: 0.7
		radius: ApplicationModel.scaleFactor * 4

		Row {
			spacing: Constants.groupbox_spacing
			padding: Constants.pane_padding / 2
			Label {
				//: LABEL DESKTOP_QML
				text: qsTr("Developer Mode: Enabled!") + SettingsModel.translationTrigger
				color: Constants.red
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: Style.dimens.normal_font_size
			}
			GButton {
				//: LABEL DESKTOP_QML Global button to disable developer mode.
				text: qsTr("Disable") + SettingsModel.translationTrigger
				onClicked: SettingsModel.developerMode = false
			}
		}
	}

	Rectangle {
		id: developerWarning

		visible: SettingsModel.developerMode && d.activeView !== SectionPage.Views.Settings
		height: ApplicationModel.scaleFactor * 50
		width: Math.sqrt(contentLoader.width * contentLoader.width + contentLoader.height * contentLoader.height)
		anchors.verticalCenter: parent.bottom

		color: Constants.red
		opacity: 0.5
		rotation: -Math.atan(contentLoader.height / contentLoader.width) * 180 / Math.PI
		transformOrigin: Item.Left
		antialiasing: true
	}
}
