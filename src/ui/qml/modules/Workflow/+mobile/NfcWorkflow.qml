/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.Type

GFlickableColumnLayout {
	id: root

	property bool cardInitiallyAppeared: false
	property bool isRemoteWorkflow: ApplicationModel.currentWorkflow === ApplicationModel.Workflow.REMOTE_SERVICE
	readonly property int nfcState: visible ? ApplicationModel.nfcState : ApplicationModel.NfcState.UNAVAILABLE

	signal showNfcInformation
	signal showRemoteServiceSettings
	signal startScanIfNecessary

	clip: true
	maximumContentWidth: Style.dimens.max_text_width
	spacing: 0
	topMargin: 0

	TechnologyInfo {
		id: technologyInfo

		//: INFO ANDROID IOS AA can't use NFC on this device, suggest to use SaC instead. The text between %1 and %2 will be emphasized.
		readonly property string pairingHint: qsTr("If you want to use the online identification on this device anyway, you can %1pair%2 another %1NFC-enabled smartphone%2 as a card reader.").arg("<b>").arg("</b>") +
		//: INFO ANDROID IOS AA can't use NFC on this device, suggest to use a PC instead. The text between %1 and %2 will be emphasized.
		(root.isRemoteWorkflow ? "" : "<br/><br/>" + qsTr("Alternatively, you may use the %3 with a %1USB card reader%2 on your %1PC%2.").arg("<b>").arg("</b>").arg(Qt.application.name))
		readonly property string positionHint: {
			if (Style.is_layout_ios) {
				//: INFO IOS The ID card may be inserted, the authentication process may be started.
				return qsTr("Please place your ID card on the top of the device's back side");
			} else {
				//: INFO ANDROID Text regarding the ID card position on the device
				return qsTr("Please place your ID card directly on the device's back side");
			}
		}

		Layout.alignment: Qt.AlignHCenter
		enableButtonText: {
			switch (root.nfcState) {
			case ApplicationModel.NfcState.UNAVAILABLE:
				//: LABEL ANDROID IOS
				return qsTr("Pair NFC-enabled smartphone");
			case ApplicationModel.NfcState.DISABLED:
				//: LABEL ANDROID IOS
				return qsTr("Go to NFC settings");
			case ApplicationModel.NfcState.INACTIVE:
				//: LABEL ANDROID IOS
				return qsTr("Start NFC scan");
			case ApplicationModel.NfcState.READY:
				if (ApplicationModel.extendedLengthApdusUnsupported && !root.isRemoteWorkflow) {
					//: LABEL ANDROID IOS
					return qsTr("Pair NFC-enabled smartphone");
				}

				return "";
			}
		}
		infoText: {
			switch (root.nfcState) {
			case ApplicationModel.NfcState.UNAVAILABLE:
				return pairingHint;
			case ApplicationModel.NfcState.DISABLED:
				//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
				return qsTr("Please enable NFC in your system settings.");
			case ApplicationModel.NfcState.INACTIVE:
				//: INFO ANDROID IOS NFC is available and enabled but needs to be started.
				return qsTr("Please start the NFC scan.");
			case ApplicationModel.NfcState.READY:
				if (ApplicationModel.extendedLengthApdusUnsupported) {
					//: INFO ANDROID IOS The NFC interface does not meet the minimum requirements.
					return qsTr("Your device does not meet the technical requirements (Extended Length not supported).") + "<br/><br/>" + pairingHint;
				}
				if (root.cardInitiallyAppeared) {
					//: INFO ANDROID IOS
					return qsTr("Please place your ID card directly on the device's back side and try several positions.") + "<br/><br/>" +
					//: INFO ANDROID IOS %1 + %2 are for bold formatting
					qsTr("Once the connection has been established, %1hold%2 the card in the %1same position%2 until you are asked to remove it again.").arg("<b>").arg("</b>");
				}
				return "%1%2".arg(root.isRemoteWorkflow ? technologyInfo.positionHint + ". <br/><br/>" : "").arg(
				//: INFO ANDROID Text that one ID card position should be kept for several seconds
				qsTr("Keep one position for several seconds before trying another one and do not move the ID card after contact was established."));
			}
		}
		showAdditionalContent: {
			switch (root.nfcState) {
			case ApplicationModel.NfcState.INACTIVE:
				return true;
			case ApplicationModel.NfcState.READY:
				return !ApplicationModel.extendedLengthApdusUnsupported;
			default:
				return false;
			}
		}
		subTitleText: {
			if (root.cardInitiallyAppeared) {
				//: INFO ANDROID IOS
				return qsTr("Ensure a stable connection to the ID card");
			}
			if (root.isRemoteWorkflow && RemoteServiceModel.connectedClientName !== "") {
				//: INFO ANDROID IOS %1 will be replaced with the name of the device.
				return qsTr("The device \"%1\" wants to use this smartphone as card reader and connect to your ID card").arg(RemoteServiceModel.connectedClientName);
			}
			if (root.nfcState === ApplicationModel.NfcState.UNAVAILABLE || ApplicationModel.extendedLengthApdusUnsupported) {
				//: INFO ANDROID IOS AA can't use NFC on this device, suggest to use SaK instead.
				return qsTr("This device cannot read the ID card");
			} else if (root.nfcState === ApplicationModel.NfcState.DISABLED) {
				//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
				return qsTr("NFC is switched off");
			} else if (root.nfcState === ApplicationModel.NfcState.INACTIVE) {
				//: INFO ANDROID IOS NFC is available and enabled but needs to be started.
				return qsTr("Start scan");
			}
			if (AuthModel.eidTypeMismatchError !== "") {
				return AuthModel.eidTypeMismatchError;
			}
			return technologyInfo.positionHint;
		}
		titleText: {
			switch (root.nfcState) {
			case ApplicationModel.NfcState.UNAVAILABLE:
				//: INFO ANDROID IOS
				return qsTr("NFC is not available");
			case ApplicationModel.NfcState.DISABLED:
				//: INFO ANDROID IOS
				return qsTr("NFC is disabled");
			case ApplicationModel.NfcState.INACTIVE:
				//: INFO ANDROID IOS
				return qsTr("NFC scan is not running");
			case ApplicationModel.NfcState.READY:
				if (ApplicationModel.extendedLengthApdusUnsupported) {
					//: INFO ANDROID IOS
					return qsTr("NFC is not available");
				}
				if (root.cardInitiallyAppeared) {
					//: INFO ANDROID IOS
					return qsTr("Connection to ID card lost");
				}
				//: INFO ANDROID IOS
				return qsTr("Read ID card");
			}
		}

		progressIndicator: NfcProgressIndicator {
			Accessible.ignored: true
			state: {
				switch (root.nfcState) {
				case ApplicationModel.NfcState.UNAVAILABLE:
					return "unavailable";
				case ApplicationModel.NfcState.READY:
					if (ApplicationModel.extendedLengthApdusUnsupported) {
						return "unavailable";
					}

					return "on";
				default:
					return "off";
				}
			}
		}

		onEnableClicked: {
			switch (root.nfcState) {
			case ApplicationModel.NfcState.UNAVAILABLE:
				root.showRemoteServiceSettings();
				break;
			case ApplicationModel.NfcState.DISABLED:
				ApplicationModel.showSettings(ApplicationModel.Settings.NFC);
				break;
			case ApplicationModel.NfcState.INACTIVE:
				root.startScanIfNecessary();
				break;
			case ApplicationModel.NfcState.READY:
				if (ApplicationModel.extendedLengthApdusUnsupported && !root.isRemoteWorkflow) {
					root.showRemoteServiceSettings();
				}
				break;
			}
		}

		MoreInformationLink {
			id: moreInformationLink

			Layout.alignment: Qt.AlignHCenter
			//: ANDROID IOS LABEL
			text: qsTr("My ID card is not detected")

			onClicked: root.showNfcInformation()
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	RowLayout {
		Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
		Layout.topMargin: Style.dimens.pane_spacing
		spacing: Style.dimens.text_spacing
		visible: root.nfcState === ApplicationModel.NfcState.READY && !ApplicationModel.extendedLengthApdusUnsupported

		GText {
			//: INFO ANDROID IOS
			text: qsTr("Searching for ID card")
			textStyle: Style.text.link
		}
		SpinningLoader {
		}
	}
}
