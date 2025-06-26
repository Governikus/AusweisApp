/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.TitleBar

CardNotActivatedBaseView {
	id: root

	onDecisionHasCodeClicked: root.pushSubView(true, {
		navigationAction: navigationAction.createObject(root)
	})
	onDecisionHasNoCodeClicked: root.pushSubView(false, {
		navigationAction: navigationAction.createObject(root)
	})

	Component {
		id: navigationAction

		NavigationAction {
			action: NavigationAction.Back

			onClicked: root.pop()
		}
	}
}
