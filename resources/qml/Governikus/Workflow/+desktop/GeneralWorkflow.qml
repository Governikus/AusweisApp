/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
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
	signal requestPasswordInfo()

	Accessible.name: qsTr("General workflow view")
	Accessible.description: qsTr("This is the general workflow view of the AusweisApp2.")

	QtObject {
		id: d

		readonly property bool foundSelectedReader: ApplicationModel.availableReader > 0
		readonly property bool foundPCSCReader: ApplicationModel.availableReader > 0 && ApplicationModel.isReaderTypeAvailable(ReaderPlugIn.PCSC)
		readonly property bool foundRemoteReader: ApplicationModel.availableReader > 0 && ApplicationModel.isReaderTypeAvailable(ReaderPlugIn.REMOTE)
	}



	Connections {
		target: ApplicationModel
		onFireCertificateRemoved: {
			//: INFO DESKTOP The paired devices was removed since it did not respond to connection attempts. It needs to be paired again if it should be used as card reader.
			ApplicationModel.showFeedback(qsTr("The device %1 was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.").arg(pDeviceName))
		}
	}

	GText {
		visible: retryCounter.visible
		anchors.horizontalCenter: retryCounter.horizontalCenter
		anchors.bottom: retryCounter.top
		anchors.bottomMargin: Constants.component_spacing

		font.bold: true
		//: LABEL DESKTOP
		text: qsTr("Attempts")
	}

	StatusIcon {
		id: retryCounter

		visible: NumberModel.retryCounter >= 0  && NumberModel.passwordType === NumberModel.PASSWORD_PIN
		height: Style.dimens.status_icon_small
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.margins: height

		activeFocusOnTab: true
		Accessible.name: qsTr("Remaining attempts:") + " " + NumberModel.retryCounter

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
		Accessible.name: qsTr("Step %1 of 3").arg(state)

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
			switch (waitingFor) {
				case Workflow.WaitingFor.Reader:
					if (ApplicationModel.extendedLengthApdusUnsupported) {
						//: ERROR DESKTOP
						return qsTr("The used card reader does not meet the technical requirements (Extended Length not supported).")
					}
					return d.foundSelectedReader
						   //: LABEL DESKTOP
						   ? qsTr("Place ID card")
						   //: LABEL DESKTOP
						   : qsTr("Connect USB card reader or smartphone")
				case Workflow.WaitingFor.Card:
					//: LABEL DESKTOP
					return qsTr("Place ID card")
				case Workflow.WaitingFor.Password:
					//: LABEL DESKTOP
					return qsTr("Information")
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
				//: INFO DESKTOP The AA2 is waiting for an ID card to be inserted into the card reader.
				return qsTr("No ID card detected. Please ensure that your ID card is placed on the card reader.")
			} else if (!d.foundPCSCReader && d.foundRemoteReader) {
				//: INFO DESKTOP The AA2 is waiting for the smartphone to be placed on the id.
				return qsTr("No ID card detected. Please make sure that the NFC interface of the smartphone (connected to %1) is correctly placed on your ID card.").arg(RemoteServiceModel.connectedServerDeviceNames)
			}

			//: INFO DESKTOP The AA2 is waiting for an ID card to be inserted into the card reader (or smartphone for that matter).
			return qsTr("Please place the smartphone (connected to %1) on your ID card or put the ID card on the card reader.").arg(RemoteServiceModel.connectedServerDeviceNames)
		}

		visible: text !== "" && !ApplicationModel.extendedLengthApdusUnsupported
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: mainText.bottom
		anchors.topMargin: Constants.text_spacing

		activeFocusOnTab: true

		textFormat: Text.StyledText
		linkColor: Style.text.header_inverse.textColor
		text: {
			switch (waitingFor) {
				case Workflow.WaitingFor.Reader:
					//: INFO DESKTOP AA2 is waiting for the card reader or the ID card.
					return d.foundSelectedReader ? requestCardText : qsTr("No card reader detected. Please make sure that an USB card reader is connected or a smartphone as card reader is paired and available. Open the %1reader settings%2 to configure readers and get more information about supported readers.").arg("<a href=\"#\">").arg("</a>")
				case Workflow.WaitingFor.Card:
					return requestCardText
				case Workflow.WaitingFor.Password:
					return ("%1<br><br><a href=\"#\">%2</a>").arg(
							   //: INFO DESKTOP The card reader is a comfort reader with its own display, the user is requested to pay attention to that display (instead of the AA2).
							   qsTr("Please observe the display of your card reader.")
						   ).arg(
							   //: INFO DESKTOP Link text
							   qsTr("More information")
						   )
				default:
					return ""
			}
		}
		textStyle: Style.text.header_secondary_inverse
		horizontalAlignment: Text.AlignHCenter

		onLinkActivated: waitingFor === Workflow.WaitingFor.Reader ? root.settingsRequested() : root.requestPasswordInfo()

		FocusFrame {}
	}
}
