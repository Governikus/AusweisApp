/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

pragma Singleton

import QtQuick

QtObject {
	function announce(pContextItem, pMessage) {
		pContextItem.Accessible.announce(pMessage);
	}
}
