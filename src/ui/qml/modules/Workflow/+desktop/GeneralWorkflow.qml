/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

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

		readonly property bool foundPCSCReader: ApplicationModel.availableReader > 0 && ApplicationModel.isReaderTypeAvailable(ReaderManagerPluginType.PCSC)
		readonly property bool foundRemoteReader: ApplicationModel.availableReader > 0 && ApplicationModel.isReaderTypeAvailable(ReaderManagerPluginType.REMOTE_IFD)
		readonly property bool foundSelectedReader: ApplicationModel.availableReader > 0

		onFoundPCSCReaderChanged: if (ApplicationModel.isScreenReaderRunning())
			subText.forceActiveFocus()
		onFoundRemoteReaderChanged: if (ApplicationModel.isScreenReaderRunning())
			subText.forceActiveFocus()
	}
	Connections {
		function onFireCertificateRemoved(pDeviceName) {
			deviceUnpaired(pDeviceName);
		}

		target: RemoteServiceModel
	}
	AnimationLoader {
		type: {
			if (waitingFor === Workflow.WaitingFor.Reader) {
				if (d.foundRemoteReader) {
					return AnimationLoader.Type.WAIT_FOR_CARD_SAC;
				}
				if (d.foundPCSCReader) {
					return AnimationLoader.Type.WAIT_FOR_CARD_USB;
				}
				return AnimationLoader.Type.WAIT_FOR_READER;
			}
			return AnimationLoader.Type.NONE;
		}

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: parent.top
			topMargin: Constants.component_spacing
		}
	}
	TintableIcon {
		source: {
			if (d.foundRemoteReader) {
				return "qrc:///images/desktop/workflow_idcard_nfc.svg";
			}
			return "qrc:///images/desktop/workflow_idcard_usb.svg";
		}
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.image
		visible: waitingFor === Workflow.WaitingFor.Password

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: parent.top
			topMargin: Constants.component_spacing
		}
	}
	ProgressCircle {
		id: progressCircle

		Accessible.focusable: true
		Accessible.name: qsTr("Step %1 of 3").arg(state)
		Accessible.role: Accessible.ProgressBar
		activeFocusOnTab: true
		state: switch (waitingFor) {
		case Workflow.WaitingFor.Reader:
			return d.foundSelectedReader ? "2" : "1";
		case Workflow.WaitingFor.Password:
			return "3";
		default:
			return "1";
		}
		visible: waitingFor !== Workflow.WaitingFor.None

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: parent.top
			topMargin: 3 * Constants.component_spacing + Style.dimens.header_icon_size
		}
		FocusFrame {
		}
	}
	GText {
		id: mainText

		Accessible.name: mainText.text
		activeFocusOnTab: true
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

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: progressCircle.bottom
			topMargin: Constants.component_spacing * 2
		}
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
		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (waitingFor) {
			case Workflow.WaitingFor.Reader:
				//: INFO DESKTOP AA2 is waiting for the card reader or the ID card.
				return d.foundSelectedReader ? requestCardText : qsTr("No card reader detected. Please make sure that an USB card reader is connected or a smartphone as card reader is paired and ready. Open the reader settings to configure readers and get more information about supported readers.");
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

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: mainText.bottom
			topMargin: Constants.text_spacing * 2
		}
		FocusFrame {
		}
	}
	GButton {
		id: readerSettingsLink

		//: INFO DESKTOP
		text: qsTr("Go to reader settings")
		visible: waitingFor === Workflow.WaitingFor.Reader && !d.foundSelectedReader

		onClicked: root.settingsRequested()

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: subText.bottom
			topMargin: Constants.component_spacing
		}
	}
}
