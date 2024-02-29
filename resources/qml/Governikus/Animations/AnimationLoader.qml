/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import Governikus.Global
import Governikus.Style

Loader {
	id: root

	enum Type {
		NONE,
		ENTER_REMOTE_PIN,
		ENTER_TRANSPORT_PIN,
		ENTER_PIN,
		ENTER_NEW_PIN,
		ENTER_CAN,
		ENTER_PUK,
		ENTER_NO_PIN,
		WAIT_FOR_CARD_SAC,
		WAIT_FOR_CARD_USB,
		WAIT_FOR_READER
	}

	property bool animated: true
	required property int type

	sourceComponent: {
		source: switch (type) {
		case AnimationLoader.Type.ENTER_REMOTE_PIN:
			return enterRemotePin;
		case AnimationLoader.Type.ENTER_TRANSPORT_PIN:
			return enterTransportPin;
		case AnimationLoader.Type.ENTER_PIN:
			return enterPin;
		case AnimationLoader.Type.ENTER_NEW_PIN:
			return enterNewPin;
		case AnimationLoader.Type.ENTER_CAN:
			return enterCan;
		case AnimationLoader.Type.ENTER_PUK:
			return enterPuk;
		case AnimationLoader.Type.ENTER_NO_PIN:
			return enterNoPin;
		case AnimationLoader.Type.WAIT_FOR_CARD_SAC:
			return waitForCardSac;
		case AnimationLoader.Type.WAIT_FOR_CARD_USB:
			return waitForCardUsb;
		case AnimationLoader.Type.WAIT_FOR_READER:
			return waitForReader;
		default:
			return undefined;
		}
	}
	visible: status === Loader.Ready || status === Loader.Loading

	Component {
		id: enterRemotePin

		EnterRemotePinAnimation {
			animated: root.animated
		}
	}
	Component {
		id: enterTransportPin

		EnterTransportPinAnimation {
			animated: root.animated
		}
	}
	Component {
		id: enterPin

		EnterPinAnimation {
			animated: root.animated
		}
	}
	Component {
		id: enterNewPin

		EnterNewPinAnimation {
			animated: root.animated
		}
	}
	Component {
		id: enterCan

		EnterCanAnimation {
			animated: root.animated
		}
	}
	Component {
		id: enterPuk

		EnterPukAnimation {
			animated: root.animated
		}
	}
	Component {
		id: enterNoPin

		EnterNoPinAnimation {
			animated: root.animated
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
		}
	}
	Component {
		id: waitForReader

		WaitForReaderAnimation {
			animated: root.animated
		}
	}
}
