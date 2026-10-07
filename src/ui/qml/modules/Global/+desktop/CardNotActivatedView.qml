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
		titleBarSettings: titleBarSettingsComponent.createObject(root)
	})
	onDecisionHasNoCodeClicked: root.pushSubView(false, {
		titleBarSettings: titleBarSettingsComponent.createObject(root)
	})

	Component {
		id: titleBarSettingsComponent

		TitleBarSettings {
			navigationAction: NavigationAction.Action.Back

			onNavigationActionClicked: root.pop()
		}
	}
}
