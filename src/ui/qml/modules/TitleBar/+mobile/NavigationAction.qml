/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

Item {
	enum Action {
		None,
		Cancel,
		Back
	}

	property int action: NavigationAction.Action.None

	signal clicked
}
