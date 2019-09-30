/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQml.Models 2.10

import Governikus.EnterPasswordView 1.0
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NumberModel  1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0

SectionPage {
	id: root

	property alias rootEnabled: titleBarAction.rootEnabled
	property bool showPairingInfoPopup: false
	signal closeView()

	enum SubView {
		None,
		EnterPassword,
		PairingInfo
	}

	QtObject {
		id: d

		property int view
	}

	onVisibleChanged: d.view = TabbedReaderView.SubView.None

	titleBarAction: TitleBarAction {
		id:titleBarAction

		//: LABEL DESKTOP_QML
		text: qsTr("Card Readers") + SettingsModel.translationTrigger
		helpTopic: "readerDeviceTab"
		rootEnabled: false
		customSubAction: CancelAction {
			visible: d.view  === TabbedReaderView.SubView.EnterPassword

			onClicked: d.view = TabbedReaderView.SubView.None
		}
	}

	TabbedPane {
		id: tabbedPane

		visible: d.view === TabbedReaderView.SubView.None

		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		sectionsModel: [
			qsTr("Smartphone as card reader") + SettingsModel.translationTrigger,
			qsTr("USB card reader") + SettingsModel.translationTrigger
		]
		contentObjectModel: ObjectModel {
			Component {
				RemoteReaderView {
					width: parent.width
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
					onPairDevice: {
						if (RemoteServiceModel.rememberServer(pDeviceId)) {
							if (showPairingInfoPopup) {
								pairingInfo.open()
							}
							else {
								d.view = TabbedReaderView.SubView.EnterPassword
								appWindow.menuBar.updateActions()
							}
						}
					}
					onUnpairDevice: RemoteServiceModel.forgetDevice(pDeviceId)
				}
			}

			Component {
				CardReaderView {
					width: parent.width
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
				}
			}
		}
		footerItem: Item {
			height: childrenRect.height

			NavigationButton {
				buttonType: Qt.BackButton
				onClicked: root.closeView()
			}
		}
	}

	ConfirmationPopup {
		id: pairingInfo

		//: LABEL DESKTOP_QML
		text: qsTr("Please start pairing mode first.") + SettingsModel.translationTrigger
		onConfirmed: {
			d.view = TabbedReaderView.SubView.EnterPassword
			appWindow.menuBar.updateActions()
		}
	}

	EnterPasswordView {
		id: enterPassword

		visible: d.view === TabbedReaderView.SubView.EnterPassword

		statusIcon: "qrc:///images/phone_to_pc.svg"
		passwordType: NumberModel.PASSWORD_REMOTE_PIN

		onPasswordEntered: d.view = TabbedReaderView.SubView.None

		onRequestPasswordInfo: {
			d.view = TabbedReaderView.SubView.PairingInfo
			appWindow.menuBar.updateActions()
		}
	}

	PasswordInfoView {
		id: passwordInfoView

		visible: d.view === TabbedReaderView.SubView.PairingInfo

		passwordType: NumberModel.PASSWORD_REMOTE_PIN

		onClose: {
			d.view = TabbedReaderView.SubView.EnterPassword
			appWindow.menuBar.updateActions()
		}
	}
}
