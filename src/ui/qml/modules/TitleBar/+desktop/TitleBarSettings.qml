/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQml

import Governikus.Type

QtObject {
	property int navigationAction: NavigationAction.None
	property bool navigationEnabled: true
	property bool showSettings: false
	property bool startEnabled: ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE

	signal navigationActionClicked
	signal settingsClicked
}
