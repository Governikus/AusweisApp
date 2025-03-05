/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Type

AnimationLoader {
	id: root

	property int animation: GAnimation.NONE

	symbol: {
		switch (animation) {
		case GAnimation.STATUS_OK:
			return Symbol.Type.CHECK;
		case GAnimation.PUK_BLOCKED:
			return Symbol.Type.WARNING;
		case GAnimation.CHANGEPIN_SUCCESS:
			return Symbol.Type.STAR;
		default:
			return Symbol.Type.ERROR;
		}
	}
	type: {
		switch (animation) {
		case GAnimation.STATUS_OK:
		case GAnimation.STATUS_ERROR:
			return AnimationLoader.STATUS;
		case GAnimation.PIN_ERROR:
			return AnimationLoader.PIN;
		case GAnimation.CAN_ERROR:
			return AnimationLoader.CAN;
		case GAnimation.PUK_ERROR:
		case GAnimation.PUK_BLOCKED:
			return AnimationLoader.PUK;
		case GAnimation.CARD_ERROR:
			return AnimationLoader.CARD_RESULT;
		case GAnimation.NETWORK_ERROR:
			return AnimationLoader.NETWORK_ERROR;
		case GAnimation.CHANGEPIN_SUCCESS:
			return AnimationLoader.CHANGEPIN_SUCCESS;
		default:
			return AnimationLoader.NONE;
		}
	}
}
