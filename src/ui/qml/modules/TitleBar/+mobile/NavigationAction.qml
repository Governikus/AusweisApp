/**
 * Copyright (c) 2019-2026 Governikus Service GmbH, Germany
 */

import QtQuick

Item {
	enum Action {
		None,
		Cancel,
		Back,
		Close
	}

	property int action: NavigationAction.Action.None

	signal clicked
}
