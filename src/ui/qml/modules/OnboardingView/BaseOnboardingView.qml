/**
 * Copyright (c) 2024-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import Governikus.Type
import Governikus.View

FlickableSectionPage {
	signal continueOnboarding

	function exitOnboarding(pSuccess = true) {
		SettingsModel.onboardingShown = true;
		if (pSuccess) {
			SettingsModel.showOnboarding = false;
		}
		leaveView();
	}
}
