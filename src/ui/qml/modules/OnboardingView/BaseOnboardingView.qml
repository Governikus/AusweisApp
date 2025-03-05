/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
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
