/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.FeedbackView 1.0
import Governikus.MainView 1.0
import Governikus.HistoryView 1.0
import Governikus.SelfAuthenticationView 1.0
import Governikus.IdentifyView 1.0
import Governikus.ChangePinView 1.0
import Governikus.ProgressView 1.0
import Governikus.ProviderView 1.0
import Governikus.MoreView 1.0
import Governikus.SettingsView 1.0
import Governikus.TutorialView 1.0
import Governikus.UpdateView 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.UiModule 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Style 1.0

import QtQml 2.10
import QtQml.Models 2.10
import QtQuick 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0 as Labs


ApplicationWindow {
	id: appWindow

	function openSaveFileDialog(pOnAcceptedCallback, pDefaultFilename, pFileType, pSuffix) {
		saveFileDialog.nameFilters = "%1 (*.%2)".arg(pFileType).arg(pSuffix)
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

	title: Qt.application.name + (plugin.developerVersion ? " - Beta - " + Qt.application.version : "")
	color: Style.color.background
	menuBar: TitleBar {
		id: titleBar
		contentRoot: contentLoader
		onRootClicked: d.activeView = SectionPage.Views.Main
	}

	Component.onCompleted: {
		if (SettingsModel.showNewUiHint) {
			useWidgetsPopup.open()
		}
		else {
			menuBar.forceActiveFocus()
		}
	}

	onWidthChanged: d.setScaleFactor()
	onHeightChanged: d.setScaleFactor()

	onClosing: {
		if (ApplicationModel.currentWorkflow !== "") {
			abortWorkflowWarning.open()
			close.accepted = false
			return
		}

		if (SettingsModel.remindUserToClose) {
			closeWarning.open()
			close.accepted = false
		} else {
			d.hideUiAndTaskbarEntry()
		}
	}
	onVisibilityChanged: if (visibility !== ApplicationWindow.Minimized) d.lastVisibility = visibility

	function showDetachedLogView() {
		if (d.detachedLogView === null) {
			d.detachedLogView = detachedLogViewWindow.createObject(appWindow)
		}
		else {
			d.detachedLogView.raise()
		}
	}

	QtObject {
		id: d

		property int activeView: SectionPage.Views.Main
		property int lastVisibility: ApplicationWindow.Windowed
		readonly property int initialWidth: 960
		readonly property int initialHeight: 720
		property ApplicationWindow detachedLogView: null

		function abortCurrentWorkflow() {
			if (ApplicationModel.currentWorkflow === "authentication") {
				AuthModel.cancelWorkflow()
			}
			else if (ApplicationModel.currentWorkflow === "selfauthentication") {
				SelfAuthModel.cancelWorkflow()
			}
			else if (ApplicationModel.currentWorkflow === "changepin") {
				ChangePinModel.cancelWorkflow()
			}
		}

		function hideUiAndTaskbarEntry() {
			hide()
			plugin.hideFromTaskbar()
		}

		function closeOpenDialogs() {
			saveFileDialog.reject()
			closeWarning.close()
			abortWorkflowWarning.close()
		}

		function showMainWindow() {
			if (active) {
				return
			}

			var currentFlags = flags
			// Force the window to the foreground if it was minimized or is behind other windows (not closed to tray)
			if (Qt.platform.os === "windows") {
				flags = currentFlags | Qt.WindowStaysOnTopHint | Qt.WindowTitleHint
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

	TaskbarProgress {}

	Labs.FileDialog {
		id: saveFileDialog

		property var acceptedCallback

		fileMode: Labs.FileDialog.SaveFile
		folder: Labs.StandardPaths.writableLocation(Labs.StandardPaths.DocumentsLocation)
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
		onConfirmed: d.hideUiAndTaskbarEntry()

		ToggleableOption {
			//: LABEL DESKTOP_QML
			text: qsTr("Do not show this dialog again.") + SettingsModel.translationTrigger
			textStyle: Style.text.normal_inverse

			checked: !SettingsModel.remindUserToClose

			onCheckedChanged: SettingsModel.remindUserToClose = !checked
		}
	}

	ConfirmationPopup {
		id: abortWorkflowWarning

		//: INFO DESKTOP_QML Content of the popup that is shown when the AA2 is closed and a workflow is still active.
		readonly property string abortText: qsTr("This will cancel the current operation and hide the UI of %1. You can restart the operation at any time.").arg(Qt.application.name) + SettingsModel.translationTrigger
		//: INFO DESKTOP_QML Content of the popup that is shown when the AA2 is closed and a workflow is still active and the close/minimize info was not disabled.
		readonly property string hideToTrayText: qsTr("The program remains available via the icon in the system tray. Click on the %1 icon to reopen the user interface.").arg(Qt.application.name) + SettingsModel.translationTrigger

		closePolicy: Popup.NoAutoClose
		//: INFO DESKTOP_QML Header of the popup that is shown when the AA2 is closed and a workflow is still active
		title: qsTr("Abort operation") + SettingsModel.translationTrigger
		text: "%1%2".arg(abortText).arg(SettingsModel.remindUserToClose ? "<br/><br/>%1".arg(hideToTrayText) : "")

		onConfirmed: {
			d.abortCurrentWorkflow()
			d.hideUiAndTaskbarEntry()
		}
		onCancelled: close()
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
				case UiModule.UPDATEINFORMATION:
					if (ApplicationModel.currentWorkflow === "" && d.activeView === SectionPage.Views.Main) {
						d.activeView = SectionPage.Views.AppUpdateInfo
					}
					break
			}
		}
		onFireHideRequest: hide()
	}

	Connections {
		target: SettingsModel
		onFireAppUpdateDataChanged: {
			if (!SettingsModel.appUpdateData.valid) {
				//: INFO DESKTOP_QML Message that the update data is invalid and can't be used.
				ApplicationModel.showFeedback(qsTr("Failed to retrieve update information."))
			}
			else if (SettingsModel.appUpdateData.updateAvailable) {
				//: INFO DESKTOP_QML An update was found which matches the current platform, the new version number is shown in the message.
				ApplicationModel.showFeedback(qsTr("An update is available (version %1).").arg(SettingsModel.appUpdateData.version))
			}
		}
	}

	Shortcut {
		sequence: StandardKey.HelpContents
		onActivated: ApplicationModel.openOnlineHelp(titleBar.rightMostAction.helpTopic)
	}

	Image {
		visible: plugin.developerVersion && SettingsModel.showBetaTesting
		height: 0.8 * parent.height
		width: 0.8 * parent.width
		anchors.centerIn: parent

		opacity: 0.2
		source: "qrc:///images/beta.svg"
		fillMode: Image.PreserveAspectFit
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
			Component {MoreView {}}
			Component {SettingsView {}}
			Component {HistoryView {}}
			Component {SetupAssistantView {}}
			Component {UpdateView { onLeaveView: d.activeView = SectionPage.Views.Main }}
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

		onItemChanged: {
			titleBar.updateActions()
			item.setActive()
		}
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

	ConfirmationPopup {
		id: useWidgetsPopup

		title: qsTr("New user interface released!") + SettingsModel.translationTrigger
		text: qsTr("This release features a new and modern user interface. For this version of %1 (and this version only!) you may decide to switch back to the previous user interface. To do so click the Button \"%2\". It is highly recommend to use the new user interface as the old one will be removed with the next release of %3.").arg(Qt.application.name).arg(cancelButtonText).arg(Qt.application.name) + SettingsModel.translationTrigger
		okButtonText: qsTr("Stick with new interface") + SettingsModel.translationTrigger
		cancelButtonText: qsTr("Use old interface") + SettingsModel.translationTrigger

		onClosed: SettingsModel.showNewUiHint = false
		onCancelled: plugin.switchUi()
	}

	Connections {
		target: plugin
		onFireProxyAuthenticationRequired: {
			proxyCredentials.credentials = pProxyCredentials
			proxyCredentials.open()
		}
	}

	ProxyCredentialsPopup {
		id: proxyCredentials
	}

	Component {
		id: detachedLogViewWindow

		ApplicationWindow {
			visible: true
			width: d.initialWidth
			height: d.initialHeight
			minimumHeight: appWindow.minimumHeight
			minimumWidth: appWindow.minimumWidth

			title: qsTr("%1 - Detached log viewer").arg(appWindow.title) + SettingsModel.translationTrigger

			DetachedLogView {
				anchors.fill: parent
				focus: true
			}

			onClosing: {
				d.detachedLogView.destroy()
				d.detachedLogView = null
			}
		}
	}
}
