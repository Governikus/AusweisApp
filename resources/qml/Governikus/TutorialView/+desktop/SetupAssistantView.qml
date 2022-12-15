/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.ResultView 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.SettingsView 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: baseItem
	enum SubViews {
		Welcome,
		AutoStartSetting,
		HistorySetting,
		CardReaderInfo,
		CardReader,
		TransportPin,
		PasswordInfo,
		Finished
	}

	isAbstract: true

	titleBarAction: TitleBarAction {
		helpTopic: "setupAssistant"
		rootEnabled: d.allowNavigation
		showSettings: false
		//: LABEL DESKTOP
		text: qsTr("Setup Assistant")

		onClicked: d.reset()
	}

	onVisibleChanged: if (visible)
		d.reset()

	QtObject {
		id: d

		property int activeView: SetupAssistantView.SubViews.Welcome
		readonly property bool allowNavigation: startupModule !== UiModule.TUTORIAL
		readonly property int startupModule: SettingsModel.startupModule

		function reset() {
			d.activeView = SetupAssistantView.SubViews.Welcome;
		}
	}
	DecisionView {
		agreeButton.iconSource: "qrc:///images/desktop/material_arrow_forward.svg"
		agreeText: ""
		mainIconSource: "qrc:///images/status_info.svg"
		//: INFO DESKTOP Welcome message when starting the setup assistant.
		questionText: qsTr("Welcome to the AusweisApp2. Please take a few moments to set up the environment to your needs. Every decision you make can later be changed in the settings menu.")
		style: DecisionView.ButtonStyle.AgreeButton
		visible: d.activeView === SetupAssistantView.SubViews.Welcome

		Component.onCompleted: setActive()
		onAgree: d.activeView = SettingsModel.autoStartAvailable ? SetupAssistantView.SubViews.AutoStartSetting : SetupAssistantView.SubViews.HistorySetting
	}
	DecisionView {
		mainIconSource: "qrc:///images/status_info.svg"
		//: INFO DESKTOP Information text why autostart of the App is advisable
		questionSubText: {
			let subText = qsTr("In order to successfully use the online identification function, %1 has to be running. It is therefore advisable to activate the auto-start after system startup.").arg(Qt.application.name);
			if (Qt.platform.os === "osx") {
				//: INFO MACOS Additional information that macOS auto-start add a symbol to the menu bar
				subText += " " + qsTr("The launch will add an icon to the menu bar.");
			}
			return subText;
		}
		//: INFO DESKTOP Question if the App shall be started automatically after boot
		questionText: qsTr("Do you want to automatically start the %1 after boot?").arg(Qt.application.name)
		visible: d.activeView === SetupAssistantView.SubViews.AutoStartSetting

		titleBarAction: TitleBarAction {
			helpTopic: "setupAssistant"
			rootEnabled: d.allowNavigation
			showSettings: false
			//: LABEL DESKTOP
			text: qsTr("Auto-start Setting")
		}

		onAgree: {
			SettingsModel.autoStartApp = true;
			d.activeView = SetupAssistantView.SubViews.HistorySetting;
		}
		onDisagree: {
			SettingsModel.autoStartApp = false;
			d.activeView = SetupAssistantView.SubViews.HistorySetting;
		}
	}
	DecisionView {
		mainIconSource: "qrc:///images/material_history.svg"
		//: INFO DESKTOP Information text which data is stored in the history record.
		questionSubText: qsTr("The history is only saved locally. You can use it to see on what date you transmitted which data to which party. After enabling the history you can view and delete the entries anytime.")
		//: INFO DESKTOP Question if the authentication history shall be stored.
		questionText: qsTr("Do you want to save a history of performed authentications on your device?")
		visible: d.activeView === SetupAssistantView.SubViews.HistorySetting

		titleBarAction: TitleBarAction {
			helpTopic: "setupAssistant"
			rootEnabled: d.allowNavigation
			showSettings: false
			//: LABEL DESKTOP
			text: qsTr("History Setting")
		}

		onAgree: {
			SettingsModel.historyEnabled = true;
			d.activeView = SetupAssistantView.SubViews.CardReaderInfo;
		}
		onDisagree: {
			SettingsModel.historyEnabled = false;
			d.activeView = SetupAssistantView.SubViews.CardReaderInfo;
		}
	}
	DecisionView {
		mainIconSource: "qrc:///images/reader/default_reader.png"
		//: INFO DESKTOP Information text why a card reader is required to use the online
		questionSubText: qsTr("In order to use the online identification feature on the computer, you need to set up a suitable smartphone or card reader before the first authentication process.")
		//: INFO DESKTOP Question if the the user wants to setup any card readers now.
		questionText: qsTr("Do you want to set up a card reader <u>now</u>?")
		tintEnabled: false
		visible: d.activeView === SetupAssistantView.SubViews.CardReaderInfo

		titleBarAction: TitleBarAction {
			helpTopic: "setupAssistant"
			rootEnabled: d.allowNavigation
			showSettings: false
			//: LABEL DESKTOP
			text: qsTr("Card Readers")
		}

		onAgree: d.activeView = SetupAssistantView.SubViews.CardReader
		onDisagree: d.activeView = SetupAssistantView.SubViews.TransportPin
	}
	TabbedReaderView {
		id: readerView
		paneAnchors.bottom: forwardButton.top
		rootEnabled: d.allowNavigation
		visible: d.activeView === SetupAssistantView.SubViews.CardReader

		onCloseView: d.activeView = SetupAssistantView.SubViews.CardReaderInfo

		NavigationButton {
			id: forwardButton
			buttonType: NavigationButton.Type.Forward
			visible: readerView.currentView === TabbedReaderView.SubView.None

			onClicked: d.activeView = SetupAssistantView.SubViews.TransportPin

			anchors {
				bottom: parent.bottom
				margins: Constants.pane_padding
				right: parent.right
			}
		}
	}
	TransportPinAssistantView {
		rootEnabled: d.allowNavigation
		visible: d.activeView === SetupAssistantView.SubViews.TransportPin

		onAgree: {
			SettingsModel.startupModule = UiModule.DEFAULT; // We don't want to show the setup assistant again, as the only subview left is "Setup assistant done"
			SettingsModel.transportPinReminder = false;
			baseItem.nextView(UiModule.PINMANAGEMENT);
		}
		onDisagree: {
			SettingsModel.startupModule = UiModule.DEFAULT;
			d.activeView = SetupAssistantView.SubViews.Finished;
		}
	}
	ResultView {
		//: INFO DESKTOP A11y button text to exit the setup assistant.
		buttonText: qsTr("Proceed to start page")
		resultType: ResultView.Type.IsSuccess
		//: INFO DESKTOP Success message after completing the setup assistant.
		text: qsTr("You have completed the setup of the AusweisApp2 successfully.")
		visible: d.activeView === SetupAssistantView.SubViews.Finished

		onNextView: pName => {
			baseItem.nextView(pName);
		}
	}
}
