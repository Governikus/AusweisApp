/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

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

SectionPage {
	id: baseItem

	enum SubViews {
		Welcome = 0,
		HistorySetting,
		CardReader,
		TransportPin,
		Finished
	}

	activeFocusOnTab: false

	isAbstract: true
	onVisibleChanged: if (visible) d.reset()
	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Setup Assistant") + SettingsModel.translationTrigger
		rootEnabled: d.allowNavigation
		showSettings: false
		helpTopic: "setupAssistant"
		onClicked: d.reset()
	}

	QtObject {
		id: d

		property int activeView: SetupAssistantView.SubViews.Welcome
		readonly property bool allowNavigation: !SettingsModel.showSetupAssistantOnStart

		function reset() {
			d.activeView = SetupAssistantView.SubViews.Welcome
		}
	}

	ResultView {
		visible: d.activeView === SetupAssistantView.SubViews.Welcome

		resultType: ResultView.Type.IsInfo
		//: INFO DESKTOP_QML Welcome message when starting the setup assistant.
		text: qsTr("Welcome to the AusweisApp2. Please take a few moments to setup the environment to your needs. Every decision you make can later be changed in the settings menu.") + SettingsModel.translationTrigger

		onNextView: d.activeView = SetupAssistantView.SubViews.HistorySetting
	}

	SetupAssistantBinaryDecisionView {
		visible: d.activeView === SetupAssistantView.SubViews.HistorySetting

		mainIconSource: "qrc:///images/desktop/main_history.svg"
		//: INFO DESKTOP_QML Question if the authentication history shall be stored.
		questionText: qsTr("Do you want to save a history of performed authentications?") + SettingsModel.translationTrigger
		//: INFO DESKTOP_QML Information text which data is stored in the history record.
		questionSubText: qsTr("The following data is saved: authentication date, service provider contact data, read data.") + SettingsModel.translationTrigger

		titleBarAction: TitleBarAction {
			//: LABEL DESKTOP_QML
			text: qsTr("History Setting") + SettingsModel.translationTrigger
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

		rootEnabled: d.allowNavigation

		onCloseView: d.activeView = SetupAssistantView.SubViews.HistorySetting

		Button {
			id: button

			anchors.bottom: parent.bottom
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
			anchors.bottomMargin: Constants.component_spacing
			width: icon.width + ApplicationModel.scaleFactor * 10 + buttonText.width
			height: icon.height

			Accessible.role: Accessible.Button
			Accessible.name: qsTr("Continue") + SettingsModel.translationTrigger

			activeFocusOnTab: true
			onClicked: d.activeView = SetupAssistantView.SubViews.TransportPin

			background: Item {
				id: content

				readonly property int imageWidth: ApplicationModel.scaleFactor * 60

				anchors.fill: parent

				GText {
					id: buttonText
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: parent.left

					//: LABEL DESKTOP_QML
					text: qsTr("Continue") + SettingsModel.translationTrigger
					textStyle: Style.text.normal
				}

				Rectangle {
					id: icon

					width: content.imageWidth
					height: content.imageWidth
					anchors.verticalCenter: parent.verticalCenter
					anchors.right: parent.right

					radius: button.width / 2
					border.width: content.imageWidth / 40
					border.color: Constants.white
					color: Style.color.transparent

					Rectangle {
						anchors.fill: parent
						anchors.margins: parent.height / 8

						radius: height / 2
						color: Qt.lighter(Constants.blue, 1.1)

						Image {
							id: image
							anchors.centerIn: parent
							source: "qrc:///images/desktop/continue_arrow.svg"

							sourceSize.height: parent.height / 2
						}
					}
				}
			}

			FocusFrame {}
		}
	}

	SetupAssistantBinaryDecisionView {
		visible: d.activeView === SetupAssistantView.SubViews.TransportPin

		mainIconSource: "qrc:///images/reader/default_reader.png"
		//: INFO DESKTOP_QML Inquiry message if the 5 digit transport PIN should be changed to an ordinary PIN (now).
		questionText: qsTr("Do you want to change your transport PIN to a personal PIN now?") + SettingsModel.translationTrigger
		//: INFO DESKTOP_QML Hint that this change may be carried out form the main menu as well and that it is required to use the online authentication feature of the id card.
		questionSubText: qsTr("This process can always be started from the main menu. The online-ID function is only usable with a personal PIN.") + SettingsModel.translationTrigger
		addRingAroundIcon: true

		titleBarAction: TitleBarAction {
			//: LABEL DESKTOP_QML
			text: qsTr("Transport PIN") + SettingsModel.translationTrigger
			rootEnabled: d.allowNavigation
			showSettings: false
			helpTopic: "setupAssistant"
		}

		onAgree: {
			NumberModel.requestTransportPin = true
			ChangePinModel.startWorkflow()
		}
		onDisagree: d.activeView = SetupAssistantView.SubViews.Finished
	}

	ResultView {
		visible: d.activeView === SetupAssistantView.SubViews.Finished

		resultType: ResultView.Type.IsSuccess
		//: INFO DESKTOP_QML Success message after completing the setup assistant.
		text: qsTr("You have completed the setup of the AusweisApp2 successfully.") + SettingsModel.translationTrigger
		onNextView: {
			SettingsModel.showSetupAssistantOnStart = false
			baseItem.nextView(pName)
		}
	}

}
