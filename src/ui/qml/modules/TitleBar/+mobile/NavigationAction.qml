/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
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
