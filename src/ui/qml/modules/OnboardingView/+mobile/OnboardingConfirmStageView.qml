/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import Governikus.TitleBar

BaseOnboardingView {
	id: root

	required property int stage

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.leaveView()
	}

	OnboardingConfirmStageBaseView {
		id: baseView

		progress: root.progress
		stage: root.stage

		onContinueOnboarding: root.continueOnboarding()
	}
}
