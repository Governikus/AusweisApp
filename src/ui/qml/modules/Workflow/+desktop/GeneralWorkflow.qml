/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Animations
import Governikus.Global
import Governikus.ResultView
import Governikus.Style
import Governikus.View
import Governikus.Type

SectionPage {
	id: root

	property int waitingFor: 0

	signal settingsRequested

	onWaitingForChanged: if (visible)
		progressCircle.forceActiveFocus(Qt.MouseFocusReason)

	QtObject {
		id: d

		readonly property bool foundPCSCReader: ApplicationModel.availableReader > 0 && ApplicationModel.isReaderTypeAvailable(ReaderManagerPluginType.PCSC)
		readonly property bool foundRemoteReader: ApplicationModel.availableReader > 0 && ApplicationModel.isReaderTypeAvailable(ReaderManagerPluginType.REMOTE_IFD)
		readonly property bool foundSelectedReader: ApplicationModel.availableReader > 0
		readonly property bool showProgressIndicator: root.progress === null || !root.progress.enabled

		onFoundPCSCReaderChanged: if (ApplicationModel.isScreenReaderRunning)
			subText.forceActiveFocus()
		onFoundRemoteReaderChanged: if (ApplicationModel.isScreenReaderRunning)
			subText.forceActiveFocus()
	}
	Connections {
		function onFireCertificateRemoved(pDeviceName) {
			root.push(deviceUnpairedView, {
				deviceName: pDeviceName
			});
		}

		target: RemoteServiceModel
	}
	AnimationLoader {
		symbol: root.waitingFor === Workflow.WaitingFor.Reader ? Symbol.Type.QUESTION : Symbol.Type.INFO
		type: {
			switch (root.waitingFor) {
			case Workflow.WaitingFor.Reader:
				if (d.foundRemoteReader) {
					return AnimationLoader.WAIT_FOR_CARD_SAC;
				}
				if (d.foundPCSCReader) {
					return AnimationLoader.WAIT_FOR_CARD_USB;
				}
				return AnimationLoader.WAIT_FOR_READER;
			case Workflow.WaitingFor.Password:
				if (d.foundRemoteReader) {
					return AnimationLoader.WAIT_FOR_SAC;
				}
				return AnimationLoader.WAIT_FOR_READER;
			default:
				return AnimationLoader.NONE;
			}
		}

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: parent.top
			topMargin: Style.dimens.pane_spacing
		}
	}
	ProgressCircle {
		id: progressCircle

		Accessible.focusable: true
		Accessible.name: qsTr("Step %1 of 3").arg(state)
		Accessible.role: Accessible.ProgressBar
		state: switch (root.waitingFor) {
		case Workflow.WaitingFor.Reader:
			return d.foundSelectedReader ? "2" : "1";
		case Workflow.WaitingFor.Password:
			return "3";
		default:
			return "1";
		}
		visible: d.showProgressIndicator && root.waitingFor !== Workflow.WaitingFor.None

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: parent.top
			topMargin: 3 * Style.dimens.pane_spacing + Style.dimens.header_icon_size
		}
	}
	GText {
		id: mainText

		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (root.waitingFor) {
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
		width: Math.min(parent.width - (2 * Style.dimens.pane_padding), Style.dimens.max_text_width)

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: progressCircle.bottom
			topMargin: Style.dimens.pane_spacing * 2
		}
	}
	GText {
		id: subText

		readonly property string requestCardText: {
			if (d.foundPCSCReader && !d.foundRemoteReader) {
				//: INFO DESKTOP The AA is waiting for an ID card to be inserted into the card reader.
				return qsTr("No ID card detected. Please ensure that your ID card is placed on the card reader.");
			} else if (!d.foundPCSCReader && d.foundRemoteReader) {
				//: INFO DESKTOP The AA is waiting for the smartphone to be placed on the id.
				return qsTr("No ID card detected. Please follow the instructions on your smartphone (connected to %1) to use it as card reader.").arg(RemoteServiceModel.connectedServerDeviceNames);
			}

			//: INFO DESKTOP The AA is waiting for an ID card to be inserted into the card reader (or smartphone for that matter).
			return qsTr("Please follow the instructions on your smartphone (connected to %1) or put the ID card on the card reader.").arg(RemoteServiceModel.connectedServerDeviceNames);
		}

		horizontalAlignment: Text.AlignHCenter
		text: {
			switch (root.waitingFor) {
			case Workflow.WaitingFor.Reader:
				//: INFO DESKTOP AA is waiting for the card reader or the ID card.
				return d.foundSelectedReader ? requestCardText : qsTr("No card reader detected. Connect an USB card reader or set up a smartphone as a card reader.");
			case Workflow.WaitingFor.Password:
				//: INFO DESKTOP The card reader is a comfort reader with its own display, the user is requested to pay attention to that display (instead of the AA).
				return qsTr("Please observe the display of your card reader.");
			default:
				return "";
			}
		}
		textFormat: Text.StyledText
		visible: text !== "" && !ApplicationModel.extendedLengthApdusUnsupported && AuthModel.eidTypeMismatchError === ""
		width: Math.min(parent.width - (2 * Style.dimens.pane_padding), Style.dimens.max_text_width)

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: mainText.bottom
			topMargin: Style.dimens.text_spacing * 2
		}
	}
	GButton {
		id: readerSettingsLink

		//: INFO DESKTOP
		text: qsTr("Set up card reader")
		visible: root.waitingFor === Workflow.WaitingFor.Reader && !d.foundSelectedReader

		onClicked: root.settingsRequested()

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: subText.bottom
			topMargin: Style.dimens.pane_spacing
		}
	}
	Component {
		id: deviceUnpairedView

		ResultView {
			required property string deviceName

			animationSymbol: Symbol.Type.ERROR
			animationType: AnimationLoader.SAC_RESULT
			//: INFO DESKTOP The paired devices was removed since it did not respond to connection attempts. It needs to be paired again if it should be used as card reader.
			text: qsTr("The device \"%1\" was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.").arg(deviceName)
			title: root.title

			onLeaveView: pop()
		}
	}
}
