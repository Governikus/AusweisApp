/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.RemoteServiceModel 1.0


SectionPage
{
	id: root

	property int waitingFor: 0
	property bool isPinChange: false
	signal settingsRequested()

	Accessible.name: qsTr("General workflow view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the general workflow view of the AusweisApp2.") + SettingsModel.translationTrigger

	QtObject {
		id: d

		readonly property bool foundSelectedReader: ApplicationModel.foundSelectedReader
		readonly property bool foundPCSCReader: ApplicationModel.foundSelectedReader && ApplicationModel.isReaderTypeAvailable(ReaderPlugIn.PCSC)
		readonly property bool foundRemoteReader: ApplicationModel.foundSelectedReader && ApplicationModel.isReaderTypeAvailable(ReaderPlugIn.REMOTE)
		readonly property string purpose: (isPinChange ?
										   //: LABEL DESKTOP_QML
										   qsTr("Change PIN") :
										   //: LABEL DESKTOP_QML
										   qsTr("Identify")
										  ) + SettingsModel.translationTrigger
	}



	Connections {
		target: ApplicationModel
		onFireCertificateRemoved: {
			//: INFO DESKTOP_QML The paired devices was removed since it did not respond to connection attempts. It needs to be paired again if it should be used as card reader.
			ApplicationModel.showFeedback(qsTr("The device %1 was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.").arg(pDeviceName))
		}
	}

	GText {
		visible: retryCounter.visible
		anchors.horizontalCenter: retryCounter.horizontalCenter
		anchors.bottom: retryCounter.top
		anchors.bottomMargin: Constants.component_spacing

		font.bold: true
		//: LABEL DESKTOP_QML
		text: qsTr("Attempts") + SettingsModel.translationTrigger
	}

	StatusIcon {
		id: retryCounter

		visible: NumberModel.retryCounter >= 0  && NumberModel.passwordType === NumberModel.PASSWORD_PIN
		height: Style.dimens.status_icon_small
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.margins: height

		activeFocusOnTab: true
		Accessible.name: qsTr("Remaining attempts:") + " " + NumberModel.retryCounter + SettingsModel.translationTrigger

		text: NumberModel.retryCounter

		FocusFrame {}
	}

	StatusIcon {
		height: Style.dimens.status_icon_large
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: parent.height / 4

		busy: true
		source: AuthModel.readerImage
	}

	ProgressCircle {
		id: progressCircle

		visible: waitingFor !== Workflow.WaitingFor.None
		anchors.top: parent.verticalCenter
		anchors.horizontalCenter: parent.horizontalCenter

		activeFocusOnTab: true
		Accessible.role: Accessible.ProgressBar
		Accessible.name: qsTr("Step %1 of 3").arg(state) + SettingsModel.translationTrigger

		state: switch (waitingFor) {
					case Workflow.WaitingFor.Reader:
						return d.foundSelectedReader ? "2" : "1"
					case Workflow.WaitingFor.Card:
						return "2"
					case Workflow.WaitingFor.Password:
						return "3"
					default:
						return "1"
		}

		FocusFrame {}
	}

	GText {
		id: mainText

		visible: text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: progressCircle.bottom
		anchors.topMargin: Constants.component_spacing

		activeFocusOnTab: true
		Accessible.name: mainText.text

		text: {
			SettingsModel.translationTrigger

			switch (waitingFor) {
				case Workflow.WaitingFor.Reader:
					if (ApplicationModel.extendedLengthApdusUnsupported) {
						//: ERROR DESKTOP_QML
						return qsTr("The used card reader does not meet the technical requirements (Extended Length not supported).")
					}
					//: LABEL DESKTOP_QML
					return d.foundSelectedReader ? d.purpose : qsTr("Searching for card reader")
				case Workflow.WaitingFor.Card:
					if (NumberModel.pinDeactivated) {
						//: LABEL DESKTOP_QML
						return qsTr("Information") + SettingsModel.translationTrigger
					}
					return d.purpose
				case Workflow.WaitingFor.Password:
					//: LABEL DESKTOP_QML
					return qsTr("Information") + SettingsModel.translationTrigger
				default:
					return ""
			}
		}
		textStyle: Style.text.header_inverse

		horizontalAlignment: Text.AlignHCenter
		FocusFrame {}
	}

	GText {
		id: subText

		readonly property string requestCardText: {
			if (d.foundPCSCReader && !d.foundRemoteReader) {
				//: INFO DESKTOP_QML The AA2 is waiting for an ID card to be inserted into the card reader.
				return qsTr("Please put the ID card on the card reader.")
			} else if (!d.foundPCSCReader && d.foundRemoteReader) {
				//: INFO DESKTOP_QML The AA2 is waiting for the smartphone to be placed on the id.
				return qsTr("Connected to %1. Please place the NFC interface of the smartphone on your ID card.").arg(RemoteServiceModel.connectedServerDeviceNames)
			}

			//: INFO DESKTOP_QML The AA2 is waiting for an ID card to be inserted into the card reader (or smartphone for that matter).
			return qsTr("Please place the smartphone (connected to %1) on your ID card or put the ID card on the card reader.").arg(RemoteServiceModel.connectedServerDeviceNames)
		}

		visible: text !== "" && !ApplicationModel.extendedLengthApdusUnsupported
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: mainText.bottom
		anchors.topMargin: Constants.text_spacing

		activeFocusOnTab: true
		Accessible.name: subText.text

		text: {
			SettingsModel.translationTrigger

			switch (waitingFor) {
				case Workflow.WaitingFor.Reader:
					//: INFO DESKTOP_QML AA2 is waiting for the card reader or the ID card.
					return d.foundSelectedReader ? requestCardText : qsTr("No card reader detected. Please make sure that a card reader is connected. Open the %1reader settings%2 to configure reader and get more information about supported readers.").arg("<a href=\"#\">").arg("</a>")
				case Workflow.WaitingFor.Card:
					if (NumberModel.pinDeactivated) {
						//: INFO DESKTOP_QML The online authentication feature of the card is disabled and needs to be activated by the authorities.
						return qsTr("The online identification function of your ID card is not activated. Please contact the authority responsible for issuing your identification card to activate the online identification function.")
					}
					return requestCardText
				case Workflow.WaitingFor.Password:
					//: INFO DESKTOP_QML The card reader is a comfort reader with its own display, the user is requested to pay attention to that display (instead of the AA2).
					return qsTr("Please observe the display of your card reader.")
				default:
					return ""
			}
		}
		textStyle: Style.text.header_secondary_inverse

		horizontalAlignment: Text.AlignHCenter

		onLinkActivated: root.settingsRequested()

		FocusFrame {}
	}
}
