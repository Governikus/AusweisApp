/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import Governikus.TitleBar

BaseOnboardingView {
	id: root

	required property int stage

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Back
		startEnabled: false

		onNavigationActionClicked: root.leaveView()
	}

	OnboardingConfirmStageBaseView {
		id: baseView

		progress: root.progress
		stage: root.stage

		onContinueOnboarding: root.continueOnboarding()
	}
}
