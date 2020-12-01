/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


Rectangle {
	id: baseItem

	property bool lockedAndHidden: true // Start in hidden state so that it doesn't slide out when the tutorial is active
	property var bottomSafeAreaMargin: plugin.safeAreaMargins.bottom

	signal resetContentArea

	function showHelp(pLockedAndHidden = false) {
		lockedAndHidden = pLockedAndHidden
		state = "help"
	}

	function showCheckIDCard(pLockedAndHidden = false) {
		lockedAndHidden = pLockedAndHidden
		state = "checkIDCard"
	}

	function showIdentify(pLockedAndHidden = false) {
		lockedAndHidden = pLockedAndHidden
		state = "identify"
	}

	function showMain(pLockedAndHidden = false) {
		lockedAndHidden = pLockedAndHidden
		state = "main"
	}

	function showPin(pLockedAndHidden = false) {
		lockedAndHidden = pLockedAndHidden
		state = "pin"
	}

	function showProvider(pLockedAndHidden = false) {
		lockedAndHidden = pLockedAndHidden
		state = "provider"
	}

	enabled: !lockedAndHidden
	visible: !lockedAndHidden
	height: lockedAndHidden ?  0 : (Style.dimens.navigation_bar_height + bottomSafeAreaMargin)

	color: Style.color.background_pane

	Component.onCompleted: state = SettingsModel.showSetupAssistantOnStart ? "tutorial" : "main"

	Behavior on height {
		enabled: appWindow.ready
		NumberAnimation {duration: Constants.animation_duration}
	}

	NavigationView {
		id: navigationView

		anchors.top: parent.top
		height: Style.dimens.navigation_bar_height

		Accessible.ignored: lockedAndHidden
	}
}
