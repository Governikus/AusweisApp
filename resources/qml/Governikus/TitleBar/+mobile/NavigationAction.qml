/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15

Item {
	enum Action {
		None,
		Cancel,
		Back
	}

	property int action: NavigationAction.Action.None

	signal clicked
}
