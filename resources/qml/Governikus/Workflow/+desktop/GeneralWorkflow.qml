/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.AuthModel
import Governikus.Type.NumberModel
import Governikus.Type.PasswordType
import Governikus.Type.ReaderPlugIn
import Governikus.Type.RemoteServiceModel

SectionPage {
	id: root

	property bool isPinChange: false
	property int waitingFor: 0

	signal deviceUnpaired(var pDeviceName)
	signal settingsRequested

	onWaitingForChanged: if (visible)
		progressCircle.forceActiveFocus(Qt.MouseFocusReason)

	QtObject {
		id: d

		readonly property bool foundPCSCReader: ApplicationModel.availableReader > 0 && ApplicationModel.isReaderTypeAvailable(ReaderPlugIn.PCSC)
		readonly property bool foundRemoteReader: ApplicationModel.availableReader > 0 && ApplicationModel.isReaderTypeAvailable(ReaderPlugIn.REMOTE_IFD)
		readonly property bool foundSelectedReader: ApplicationModel.availableReader > 0
	}
	Connections {
		function onFireCertificateRemoved(pDeviceName) {
			deviceUnpaired(pDeviceName);
		}

		target: RemoteServiceModel
	}
	GText {
		Accessible.ignored: true
		anchors.bottom: retryCounter.top
		anchors.bottomMargin: Constants.component_spacing
		anchors.horizontalCenter: retryCounter.horizontalCenter
		font.bold: true
		//: LABEL DESKTOP
		text: qsTr("Attempts")
		visible: retryCounter.visible
	}
	RetryCounter {
		id: retryCounter

		anchors.left: parent.left
		anchors.margins: height
		anchors.top: parent.top
		visible: NumberModel.retryCounter >= 0 && NumberModel.passwordType === PasswordType.PIN
	}
	TintableAnimation {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: parent.height / 4
		height: Style.dimens.header_icon_size
		source: {
			if (d.foundRemoteReader) {
				return "qrc:///images/desktop/workflow_waitfor_idcard_sak.webp";
			}
			if (d.foundPCSCReader) {
				return "qrc:///images/desktop/workflow_waitfor_idcard_usb.webp";
			}
			return "qrc:///images/desktop/workflow_waitfor_reader.webp";
		}
		tintColor: Style.color.control
		visible: waitingFor === Workflow.WaitingFor.Reader
	}
	TintableIcon {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: parent.height / 4
		source: {
			if (d.foundRemoteReader) {
				return "qrc:///images/desktop/workflow_idcard_nfc.svg";
			}
			return "qrc:///images/desktop/workflow_idcard_usb.svg";
		}
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.control
		visible: waitingFor === Workflow.WaitingFor.Password
	}
	ProgressCircle {
		id: progressCircle

		Accessible.focusable: true
		Accessible.name: qsTr("Step %1 of 3").arg(state)
		Accessible.role: Accessible.ProgressBar
		activeFocusOnTab: true
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.verticalCenter
		state: switch (waitingFor) {
		case Workflow.WaitingFor.Reader:
			return d.foundSelectedReader ? "2" : "1";
		case Workflow.WaitingFor.Password:
			return "3";
		default:
			return "1";
		}
		visible: waitingFor !== Workflow.WaitingFor.None

		FocusFrame {
		}
	}
	GText {
		id: mainText

		Accessible.name: mainText.text
		activeFocusOnTab: true
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: progressCircle.bottom
		anchors.topMargin: Constants.component_spacing
		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (waitingFor) {
			case Workflow.WaitingFor.Reader:
				if (!!AuthModel.eidTypeMismatchError) {
					return AuthModel.eidTypeMismatchError;
				}
				if (ApplicationModel.extendedLengthApdusUnsupported) {
					//: ERROR DESKTOP
					return qsTr("The used card reader does not meet the technical requirements (Extended Length not supported).");
				}
				return d.foundSelectedReader ?
				//: LABEL DESKTOP
				qsTr("Place ID card") :
				//: LABEL DESKTOP
				qsTr("Connect USB card reader or smartphone");
			case Workflow.WaitingFor.Password:
				//: LABEL DESKTOP
				return qsTr("Information");
			default:
				return "";
			}
		}
		textStyle: Style.text.headline
		visible: text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		FocusFrame {
		}
	}
	GText {
		id: subText

		readonly property string requestCardText: {
			if (d.foundPCSCReader && !d.foundRemoteReader) {
				//: INFO DESKTOP The AA2 is waiting for an ID card to be inserted into the card reader.
				return qsTr("No ID card detected. Please ensure that your ID card is placed on the card reader.");
			} else if (!d.foundPCSCReader && d.foundRemoteReader) {
				//: INFO DESKTOP The AA2 is waiting for the smartphone to be placed on the id.
				return qsTr("No ID card detected. Please follow the instructions on your smartphone (connected to %1) to use it as card reader.").arg(RemoteServiceModel.connectedServerDeviceNames);
			}

			//: INFO DESKTOP The AA2 is waiting for an ID card to be inserted into the card reader (or smartphone for that matter).
			return qsTr("Please follow the instructions on your smartphone (connected to %1) or put the ID card on the card reader.").arg(RemoteServiceModel.connectedServerDeviceNames);
		}

		activeFocusOnTab: true
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: mainText.bottom
		anchors.topMargin: Constants.text_spacing
		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (waitingFor) {
			case Workflow.WaitingFor.Reader:
				//: INFO DESKTOP AA2 is waiting for the card reader or the ID card.
				return d.foundSelectedReader ? requestCardText : qsTr("No card reader detected. Please make sure that an USB card reader is connected or a smartphone as card reader is paired and available. Open the reader settings to configure readers and get more information about supported readers.");
			case Workflow.WaitingFor.Password:
				//: INFO DESKTOP The card reader is a comfort reader with its own display, the user is requested to pay attention to that display (instead of the AA2).
				return qsTr("Please observe the display of your card reader.");
			default:
				return "";
			}
		}
		textFormat: Text.StyledText
		visible: text !== "" && !ApplicationModel.extendedLengthApdusUnsupported && AuthModel.eidTypeMismatchError === ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		FocusFrame {
		}
	}
	GButton {
		id: readerSettingsLink

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: subText.bottom
		anchors.topMargin: Constants.component_spacing

		//: INFO DESKTOP
		text: qsTr("Go to reader settings")
		visible: waitingFor === Workflow.WaitingFor.Reader && !d.foundSelectedReader

		onClicked: root.settingsRequested()
	}
}
