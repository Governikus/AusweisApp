/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQml

import Governikus.Type

QtObject {
	property int navigationAction: NavigationAction.Action.None
	property bool navigationEnabled: true
	property bool showSettings: false
	property bool startEnabled: ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE

	signal navigationActionClicked
	signal settingsClicked
}
