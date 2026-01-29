/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
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
	readonly property int nfcState: visible ? ApplicationModel.nfcState : nfcStateInvisible
	readonly property int nfcStateInvisible: -1

	signal showNfcInformation
	signal showRemoteServiceSettings
	signal startScanIfNecessary

	clip: true
	maximumContentWidth: Style.dimens.max_text_width
	spacing: 0
	topMargin: 0

	TechnologyInfo {
		id: technologyInfo

		//: MOBILE AA can't use NFC on this device, suggest to use SaC instead. The text between %1 and %2 will be emphasized.
		readonly property string pairingHint: qsTr("If you want to use the online identification on this device anyway, you can %1pair%2 another %1NFC-enabled smartphone%2 as a card reader.").arg("<b>").arg("</b>") +
		//: MOBILE AA can't use NFC on this device, suggest to use a PC instead. The text between %1 and %2 will be emphasized.
		(root.isRemoteWorkflow ? "" : "<br/><br/>" + qsTr("Alternatively, you may use the %3 with a %1USB card reader%2 on your %1PC%2.").arg("<b>").arg("</b>").arg(Qt.application.name))
		readonly property string positionHint: {
			if (Style.is_layout_ios) {
				//: IOS The ID card may be inserted, the authentication process may be started.
				return qsTr("Please place your ID card on the top of the device's back side");
			} else {
				//: ANDROID Text regarding the ID card position on the device
				return qsTr("Please place your ID card directly on the device's back side");
			}
		}

		Layout.alignment: Qt.AlignHCenter
		enableButtonText: {
			switch (root.nfcState) {
			case ApplicationModel.NfcState.UNAVAILABLE:
				//: MOBILE
				return qsTr("Pair NFC-enabled smartphone");
			case ApplicationModel.NfcState.DISABLED:
				//: MOBILE
				return qsTr("Go to NFC settings");
			case ApplicationModel.NfcState.INACTIVE:
				//: MOBILE
				return qsTr("Start NFC scan");
			case ApplicationModel.NfcState.READY:
				if (ApplicationModel.extendedLengthApdusUnsupported && !root.isRemoteWorkflow) {
					//: MOBILE
					return qsTr("Pair NFC-enabled smartphone");
				}

				return "";
			default:
				return "";
			}
		}
		infoText: {
			switch (root.nfcState) {
			case ApplicationModel.NfcState.UNAVAILABLE:
				return pairingHint;
			case ApplicationModel.NfcState.DISABLED:
				//: MOBILE NFC is available but needs to be activated in the settings of the smartphone.
				return qsTr("Please enable NFC in your system settings.");
			case ApplicationModel.NfcState.INACTIVE:
				//: MOBILE NFC is available and enabled but needs to be started.
				return qsTr("Please start the NFC scan.");
			case ApplicationModel.NfcState.READY:
				if (ApplicationModel.extendedLengthApdusUnsupported) {
					//: MOBILE The NFC interface does not meet the minimum requirements.
					return qsTr("Your device does not meet the technical requirements (Extended Length not supported).") + "<br/><br/>" + pairingHint;
				}
				if (root.cardInitiallyAppeared) {
					//: MOBILE
					return qsTr("Please place your ID card directly on the device's back side and try several positions.") + "<br/><br/>" +
					//: MOBILE %1 + %2 are for bold formatting
					qsTr("Once the connection has been established, %1hold%2 the card in the %1same position%2 until you are asked to remove it again.").arg("<b>").arg("</b>");
				}
				return "%1%2".arg(root.isRemoteWorkflow ? technologyInfo.positionHint + ". <br/><br/>" : "").arg(
				//: ANDROID Text that one ID card position should be kept for several seconds
				qsTr("Keep one position for several seconds before trying another one and do not move the ID card after contact was established."));
			default:
				return "";
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
				//: MOBILE
				return qsTr("Ensure a stable connection to the ID card");
			}
			if (root.isRemoteWorkflow && RemoteServiceModel.connectedClientName !== "") {
				//: MOBILE %1 will be replaced with the name of the device.
				return qsTr("The device \"%1\" wants to use this smartphone as card reader and connect to your ID card").arg(RemoteServiceModel.connectedClientName);
			}
			if (root.nfcState === ApplicationModel.NfcState.UNAVAILABLE || ApplicationModel.extendedLengthApdusUnsupported) {
				//: MOBILE AA can't use NFC on this device, suggest to use SaK instead.
				return qsTr("This device cannot read the ID card");
			} else if (root.nfcState === ApplicationModel.NfcState.DISABLED) {
				//: MOBILE NFC is available but needs to be activated in the settings of the smartphone.
				return qsTr("NFC is switched off");
			} else if (root.nfcState === ApplicationModel.NfcState.INACTIVE) {
				//: MOBILE NFC is available and enabled but needs to be started.
				return qsTr("Start scan");
			} else if (root.nfcState === root.nfcStateInvisible) {
				return "";
			}
			if (AuthModel.eidTypeMismatchError !== "") {
				return AuthModel.eidTypeMismatchError;
			}
			return technologyInfo.positionHint;
		}
		titleText: {
			switch (root.nfcState) {
			case ApplicationModel.NfcState.UNAVAILABLE:
				//: MOBILE
				return qsTr("NFC is not available");
			case ApplicationModel.NfcState.DISABLED:
				//: MOBILE
				return qsTr("NFC is disabled");
			case ApplicationModel.NfcState.INACTIVE:
				//: MOBILE
				return qsTr("NFC scan is not running");
			case ApplicationModel.NfcState.READY:
				if (ApplicationModel.extendedLengthApdusUnsupported) {
					//: MOBILE
					return qsTr("NFC is not available");
				}
				if (root.cardInitiallyAppeared) {
					//: MOBILE
					return qsTr("Connection to ID card lost");
				}
				//: MOBILE
				return qsTr("Read ID card");
			default:
				return "";
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
			//: MOBILE
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
			//: MOBILE
			text: qsTr("Searching for ID card")
			textStyle: Style.text.link
		}
		SpinningLoader {
		}
	}
}
