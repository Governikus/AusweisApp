/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


Rectangle {
	id: baseItem

	property bool lockedAndHidden: true // Start in hidden state so that it doesn't slide out when the tutorial is active
	property bool isOpen: true
	property int currentIndex: 0
	property var bottomSafeAreaMargin: plugin.safeAreaMargins.bottom

	signal reselectedState

	function open() {}

	function close() {}

	enabled: !lockedAndHidden
	visible: !lockedAndHidden
	height: lockedAndHidden ?  0 : (Style.dimens.tabbar_height + bottomSafeAreaMargin)

	color: Constants.white

	Component.onCompleted: {
		state = SettingsModel.showSetupAssistantOnStart ? "tutorial" : "identify"
	}

	Behavior on height {
		NumberAnimation {duration: Constants.animation_duration}
	}

	NavigationView {
		anchors.top: parent.top
		height: Style.dimens.tabbar_height

		Accessible.ignored: lockedAndHidden
	}
}
