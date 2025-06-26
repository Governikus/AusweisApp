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
		titleBarSettings: titleBarSettings.createObject(root)
	})
	onDecisionHasNoCodeClicked: root.pushSubView(false, {
		titleBarSettings: titleBarSettings.createObject(root)
	})

	Component {
		id: titleBarSettings

		TitleBarSettings {
			navigationAction: NavigationAction.Back

			onNavigationActionClicked: root.pop()
		}
	}
}
