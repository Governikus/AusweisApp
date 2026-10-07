/**
 * Copyright (c) 2024-2026 Governikus Service GmbH, Germany
 */

import Governikus.TitleBar

BaseSkipOnboardingConfirmation {
	id: root

	descriptionTextsModel: [
		//: DESKTOP
		qsTr("Only skip this step, if you already set up a 6-digit ID card PIN and a card reader."), root.commonDescriptionText]

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Action.Back
		startEnabled: false

		onNavigationActionClicked: root.leaveView()
	}
}
