/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

Loader {
	id: root

	enum Type {
		NONE,
		REMOTE_PIN,
		TRANSPORT_PIN,
		PIN,
		PIN_UNKNOWN,
		NEW_PIN,
		CAN,
		PUK,
		WAIT_FOR_CARD_SAC,
		WAIT_FOR_CARD_USB,
		WAIT_FOR_READER,
		WAIT_FOR_SAC,
		NFC_RESULT,
		SAC_RESULT,
		CHANGEPIN_SUCCESS,
		SAC_CONNECTION,
		CARD_RESULT,
		NETWORK_ERROR,
		STATUS,
		HOURGLASS
	}

	property bool animated: true
	property int symbol: Symbol.Type.STAR
	property int type: AnimationLoader.NONE

	sourceComponent: {
		switch (type) {
		case AnimationLoader.REMOTE_PIN:
			return remotePin;
		case AnimationLoader.TRANSPORT_PIN:
			return transportPin;
		case AnimationLoader.PIN:
			return pin;
		case AnimationLoader.PIN_UNKNOWN:
			return pinUnknown;
		case AnimationLoader.NEW_PIN:
			return newPin;
		case AnimationLoader.CAN:
			return can;
		case AnimationLoader.PUK:
			return puk;
		case AnimationLoader.WAIT_FOR_CARD_SAC:
			return waitForCardSac;
		case AnimationLoader.WAIT_FOR_CARD_USB:
			return waitForCardUsb;
		case AnimationLoader.WAIT_FOR_READER:
			return waitForReader;
		case AnimationLoader.WAIT_FOR_SAC:
			return waitForSac;
		case AnimationLoader.NFC_RESULT:
			return nfcResult;
		case AnimationLoader.SAC_RESULT:
			return sacResult;
		case AnimationLoader.CHANGEPIN_SUCCESS:
			return changePinSuccess;
		case AnimationLoader.SAC_CONNECTION:
			return sacConnection;
		case AnimationLoader.CARD_RESULT:
			return cardResult;
		case AnimationLoader.NETWORK_ERROR:
			return networkError;
		case AnimationLoader.STATUS:
			return statusAnimation;
		case AnimationLoader.HOURGLASS:
			return hourglassAnimation;
		default:
			return undefined;
		}
	}
	visible: (status === Loader.Ready || status === Loader.Loading) && type !== AnimationLoader.NONE

	Component {
		id: remotePin

		RemotePinAnimation {
			animated: root.animated
			symbol: root.symbol
		}
	}
	Component {
		id: transportPin

		TransportPinAnimation {
			animated: root.animated
			symbol: root.symbol
		}
	}
	Component {
		id: pin

		PinAnimation {
			animated: root.animated
			symbol: root.symbol
		}
	}
	Component {
		id: pinUnknown

		PinUnknownAnimation {
			animated: root.animated
			symbol: root.symbol
		}
	}
	Component {
		id: newPin

		NewPinAnimation {
			animated: root.animated
			symbol: root.symbol
		}
	}
	Component {
		id: can

		CanAnimation {
			animated: root.animated
			symbol: root.symbol
		}
	}
	Component {
		id: puk

		PukAnimation {
			animated: root.animated
			symbol: root.symbol
		}
	}
	Component {
		id: waitForCardSac

		WaitForCardSacAnimation {
			animated: root.animated
		}
	}
	Component {
		id: waitForCardUsb

		WaitForCardUsbAnimation {
			animated: root.animated
			symbol.type: root.symbol
		}
	}
	Component {
		id: waitForReader

		WaitForReaderAnimation {
			animated: root.animated
			symbol.type: root.symbol
		}
	}
	Component {
		id: waitForSac

		WaitForSacAnimation {
			animated: root.animated
			symbol.type: root.symbol
		}
	}
	Component {
		id: nfcResult

		ResultNfcAnimation {
			animated: root.animated
			symbol.type: root.symbol
		}
	}
	Component {
		id: sacResult

		ResultSacAnimation {
			animated: root.animated
			symbol.type: root.symbol
		}
	}
	Component {
		id: changePinSuccess

		ResultChangePinAnimation {
			animated: root.animated
		}
	}
	Component {
		id: sacConnection

		ResultSacConnectionAnimation {
			animated: root.animated
			symbol.type: root.symbol
		}
	}
	Component {
		id: cardResult

		ResultCardAnimation {
			animated: root.animated
			symbol.type: root.symbol
		}
	}
	Component {
		id: networkError

		ResultNetworkAnimation {
			animated: root.animated
		}
	}
	Component {
		id: statusAnimation

		StatusAnimation {
			symbol.type: root.symbol
		}
	}
	Component {
		id: hourglassAnimation

		HourglassAnimation {
		}
	}
}
