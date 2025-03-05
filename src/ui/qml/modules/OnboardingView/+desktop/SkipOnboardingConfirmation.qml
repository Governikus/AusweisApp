/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import Governikus.TitleBar

BaseSkipOnboardingConfirmation {
	id: root

	descriptionTextsModel: [
		//: LABEL DESKTOP
		qsTr("Only skip this step, if you already set up a 6-digit ID card PIN and a card reader."), root.commonDescriptionText]

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Back
		startEnabled: false

		onNavigationActionClicked: root.leaveView()
	}
}
