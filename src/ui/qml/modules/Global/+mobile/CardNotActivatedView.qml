/**
 * Copyright (c) 2025-2026 Governikus Service GmbH, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.TitleBar

CardNotActivatedBaseView {
	id: root

	onDecisionHasCodeClicked: root.pushSubView(true, {
		navigationAction: navigationActionComponent.createObject(root)
	})
	onDecisionHasNoCodeClicked: root.pushSubView(false, {
		navigationAction: navigationActionComponent.createObject(root)
	})

	Component {
		id: navigationActionComponent

		NavigationAction {
			action: NavigationAction.Action.Back

			onClicked: root.pop()
		}
	}
}
