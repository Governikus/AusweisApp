/*
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

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
		Welcome = 0,
		HistorySetting,
		CardReader,
		TransportPin,
		PasswordInfo,
		Finished
	}

	isAbstract: true

	onVisibleChanged: if (visible) d.reset()
	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Setup Assistant")
		rootEnabled: d.allowNavigation
		showSettings: false
		helpTopic: "setupAssistant"
		onClicked: d.reset()
	}

	QtObject {
		id: d

		property int activeView: SetupAssistantView.SubViews.Welcome
		readonly property int startupModule: SettingsModel.startupModule
		readonly property bool allowNavigation: startupModule !== UiModule.TUTORIAL

		function reset() {
			d.activeView = SetupAssistantView.SubViews.Welcome
		}
	}

	ResultView {
		visible: d.activeView === SetupAssistantView.SubViews.Welcome

		Component.onCompleted: setActive()

		Accessible.name: qsTr("Setup assistant view")
		Accessible.description: qsTr("This is the setup assistant view which will guide you through the basic setup of %1").arg(Qt.application.name)

		resultType: ResultView.Type.IsInfo
		//: INFO DESKTOP_QML Welcome message when starting the setup assistant.
		text: qsTr("Welcome to the AusweisApp2. Please take a few moments to setup the environment to your needs. Every decision you make can later be changed in the settings menu.")

		onNextView: d.activeView = SetupAssistantView.SubViews.HistorySetting
	}

	DecisionView {
		visible: d.activeView === SetupAssistantView.SubViews.HistorySetting

		Accessible.name: qsTr("History setup step")

		mainIconSource: "qrc:///images/history.svg"
		//: INFO DESKTOP_QML Question if the authentication history shall be stored.
		questionText: qsTr("Do you want to save a history of performed authentications on your device?")
		//: INFO DESKTOP_QML Information text which data is stored in the history record.
		questionSubText: qsTr("The history is only saved locally. You can use it to see on what date you transmitted which data to which party. After enabling the history you can view and delete the entries anytime.")

		titleBarAction: TitleBarAction {
			//: LABEL DESKTOP_QML
			text: qsTr("History Setting")
			rootEnabled: d.allowNavigation
			showSettings: false
			helpTopic: "setupAssistant"
		}

		onAgree: {
			SettingsModel.historyEnabled = true
			d.activeView = SetupAssistantView.SubViews.CardReader
		}

		onDisagree: {
			SettingsModel.historyEnabled = false
			d.activeView = SetupAssistantView.SubViews.CardReader
		}
	}

	TabbedReaderView {
		id: readerView

		visible: d.activeView === SetupAssistantView.SubViews.CardReader

		Accessible.name: qsTr("Card reader setup step")

		rootEnabled: d.allowNavigation

		onCloseView: d.activeView = SetupAssistantView.SubViews.HistorySetting

		paneAnchors.bottom: forwardButton.top

		NavigationButton {
			id: forwardButton

			visible: readerView.currentView === TabbedReaderView.SubView.None

			anchors {
				right: parent.right
				bottom: parent.bottom
				margins: Constants.pane_padding
			}

			buttonType: NavigationButton.Type.Forward
			onClicked: d.activeView = SetupAssistantView.SubViews.TransportPin
		}
	}

	TransportPinAssistantView {
		visible: d.activeView === SetupAssistantView.SubViews.TransportPin

		rootEnabled: d.allowNavigation

		onAgree: {
			SettingsModel.startupModule = UiModule.DEFAULT // We don't wan't to show the setup assistant again, as the only subview left is "Setup assistant done"
			SettingsModel.transportPinReminder = false
			baseItem.nextView(UiModule.PINMANAGEMENT)
		}
		onDisagree: {
			SettingsModel.startupModule = UiModule.DEFAULT
			d.activeView = SetupAssistantView.SubViews.Finished
		}
	}

	ResultView {
		visible: d.activeView === SetupAssistantView.SubViews.Finished

		Accessible.name: qsTr("Setup assistant done")

		resultType: ResultView.Type.IsSuccess
		//: INFO DESKTOP_QML Success message after completing the setup assistant.
		text: qsTr("You have completed the setup of the AusweisApp2 successfully.")
		onNextView: baseItem.nextView(pName)
	}

}
