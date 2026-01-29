/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
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
			return AnimationLoader.Type.STATUS;
		case GAnimation.PIN_ERROR:
			return AnimationLoader.Type.PIN;
		case GAnimation.CAN_ERROR:
			return AnimationLoader.Type.CAN;
		case GAnimation.PUK_ERROR:
		case GAnimation.PUK_BLOCKED:
			return AnimationLoader.Type.PUK;
		case GAnimation.CARD_ERROR:
			return AnimationLoader.Type.CARD_RESULT;
		case GAnimation.NETWORK_ERROR:
			return AnimationLoader.Type.NETWORK_ERROR;
		case GAnimation.CHANGEPIN_SUCCESS:
			return AnimationLoader.Type.CHANGEPIN_SUCCESS;
		default:
			return AnimationLoader.Type.NONE;
		}
	}
}
