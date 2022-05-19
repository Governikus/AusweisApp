/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
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

import QtQml 2.12
import QtQml.Models 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12
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

	minimumWidth: 480
	minimumHeight: 360

	title: Qt.application.name + (plugin.developerVersion ? " - Beta - " + Qt.application.version : "")
	color: Style.color.background
	menuBar: TitleBar {
		id: titleBar
		contentRoot: contentLoader
		onRootClicked: d.activeView = UiModule.DEFAULT
	}

	Component.onCompleted: menuBar.forceActiveFocus()

	onWidthChanged: d.setScaleFactor()
	onHeightChanged: d.setScaleFactor()

	onClosing: {
		if (ApplicationModel.currentWorkflow !== "") {
			abortWorkflowWarning.open()
			close.accepted = false
			return
		}

		if (Qt.platform.os === "osx" && !SettingsModel.autoStartApp) {
			close.accepted = true
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

		property int activeView: UiModule.DEFAULT
		property int lastVisibility: ApplicationWindow.Windowed
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
			let initialSize = plugin.initialWindowSize
			ApplicationModel.scaleFactor = Math.min(width / initialSize.width, height / initialSize.height)
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
		//: INFO DESKTOP The AA2 is currently remote controlled via the SDK interface, concurrent usage of the AA2 is not possible.
		title: qsTr("Another application uses %1").arg(Qt.application.name)
		text: plugin.dominator
		closePolicy: Popup.NoAutoClose
	}

	ConfirmationPopup {
		id: closeWarning

		closePolicy: Popup.NoAutoClose
		style: ConfirmationPopup.PopupStyle.OkButton
		//: INFO DESKTOP Header of the popup that is shown when the AA2 is closed for the first time.
		title: qsTr("The user interface of the %1 is closed.").arg(Qt.application.name)
		//: INFO DESKTOP Content of the popup that is shown when the AA2 is closed for the first time.
		text: qsTr("The program remains available via the icon in the system tray. Click on the %1 icon to reopen the user interface.").arg(Qt.application.name)
		onConfirmed: d.hideUiAndTaskbarEntry()

		ToggleableOption {
			//: LABEL DESKTOP
			text: qsTr("Do not show this dialog again.")
			textStyle: Style.text.normal_inverse

			checked: !SettingsModel.remindUserToClose

			onCheckedChanged: SettingsModel.remindUserToClose = !checked
		}
	}

	ConfirmationPopup {
		id: abortWorkflowWarning

		//: INFO DESKTOP Content of the popup that is shown when the AA2 is closed and a workflow is still active.
		readonly property string abortText: qsTr("This will cancel the current operation and hide the UI of %1. You can restart the operation at any time.").arg(Qt.application.name)
		//: INFO DESKTOP Content of the popup that is shown when the AA2 is closed and a workflow is still active and the close/minimize info was not disabled.
		readonly property string hideToTrayText: qsTr("The program remains available via the icon in the system tray. Click on the %1 icon to reopen the user interface.").arg(Qt.application.name)

		closePolicy: Popup.NoAutoClose
		//: INFO DESKTOP Header of the popup that is shown when the AA2 is closed and a workflow is still active
		title: qsTr("Abort operation")
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
				case UiModule.CURRENT:
					break
				case UiModule.IDENTIFY:
					if (ApplicationModel.currentWorkflow === "") {
						d.activeView = UiModule.SELF_AUTHENTICATION
					}
					if (ApplicationModel.currentWorkflow === "authentication" || ApplicationModel.currentWorkflow === "selfauthentication") {
						d.activeView = UiModule.IDENTIFY
					}
					break
				case UiModule.PINMANAGEMENT:
					if (ApplicationModel.currentWorkflow === "" || ApplicationModel.currentWorkflow === "changepin") {
						d.activeView = UiModule.PINMANAGEMENT
					}
					break
				case UiModule.UPDATEINFORMATION:
					if (ApplicationModel.currentWorkflow === "" && d.activeView === UiModule.DEFAULT) {
						d.activeView = UiModule.UPDATEINFORMATION
					}
					break
				default:
					if (ApplicationModel.currentWorkflow === "") {
						d.activeView = pModule
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
				//: INFO DESKTOP Message that the update data is invalid and can't be used.
				ApplicationModel.showFeedback(qsTr("Failed to retrieve update information."))
			}
			else if (SettingsModel.appUpdateData.updateAvailable) {
				//: INFO DESKTOP An update was found which matches the current platform, the new version number is shown in the message.
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
		source: "qrc:///images/logo_beta_testing.svg"
		fillMode: Image.PreserveAspectFit
	}

	Loader {
		id: contentLoader

		Component {id: main; MainView {}}
		Component {id: selfauthentication; SelfAuthenticationView {}}
		Component {id: identify; IdentifyView {}}
		Component {id: pinmanagement; ChangePinView {}}
		Component {id: provider; ProviderView {}}
		Component {id: help; MoreView {}}
		Component {id: settings; SettingsView {}}
		Component {id: history; HistoryView {}}
		Component {id: tutorial; SetupAssistantView {}}
		Component {id: updateinformation; UpdateView { onLeaveView: d.activeView = UiModule.DEFAULT }}

		Keys.onEscapePressed: {
			if (d.activeView !== UiModule.DEFAULT) {
				d.activeView = UiModule.DEFAULT
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
		sourceComponent: switch(d.activeView) {
			case UiModule.SELF_AUTHENTICATION: return selfauthentication
			case UiModule.IDENTIFY: return identify
			case UiModule.PINMANAGEMENT: return pinmanagement
			case UiModule.PROVIDER: return provider
			case UiModule.HELP: return help
			case UiModule.SETTINGS: return settings
			case UiModule.HISTORY: return history
			case UiModule.TUTORIAL: return tutorial
			case UiModule.UPDATEINFORMATION: return updateinformation
			default: return main
		}
	}

	Rectangle {
		visible: SettingsModel.developerMode && d.activeView !== UiModule.SETTINGS
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
				//: LABEL DESKTOP
				text: qsTr("Developer Mode: Enabled!")
				color: Constants.red
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: Style.dimens.normal_font_size
			}
			GButton {
				//: LABEL DESKTOP Global button to disable developer mode.
				text: qsTr("Disable")
				onClicked: SettingsModel.developerMode = false
			}
		}
	}

	Rectangle {
		id: developerWarning

		visible: SettingsModel.developerMode && d.activeView !== UiModule.SETTINGS
		height: ApplicationModel.scaleFactor * 50
		width: Math.sqrt(contentLoader.width * contentLoader.width + contentLoader.height * contentLoader.height)
		anchors.verticalCenter: parent.bottom

		color: Constants.red
		opacity: 0.5
		rotation: -Math.atan(contentLoader.height / contentLoader.width) * 180 / Math.PI
		transformOrigin: Item.Left
		antialiasing: true
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
			width: plugin.initialWindowSize.width
			height: plugin.initialWindowSize.height
			minimumHeight: appWindow.minimumHeight
			minimumWidth: appWindow.minimumWidth

			title: qsTr("%1 - Detached log viewer").arg(appWindow.title)

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
