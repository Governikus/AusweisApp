/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.UiModule 1.0

Rectangle {
	id: baseItem

	readonly property int activeModule: d.activeModule
	property bool lockedAndHidden

	signal resetContentArea

	function show(pModule, pLockedAndHidden = false) {
		lockedAndHidden = pLockedAndHidden
		if (d.activeModule !== pModule) {
			d.activeModule = pModule

			SettingsModel.startupModule = pModule === UiModule.REMOTE_SERVICE ? UiModule.REMOTE_SERVICE : UiModule.DEFAULT
		}
	}

	enabled: !lockedAndHidden
	visible: height > 0
	height: lockedAndHidden ? 0 : (Style.dimens.navigation_bar_height + plugin.safeAreaMargins.bottom)

	color: Style.color.background_pane

	Behavior on height {
		enabled: appWindow.ready
		NumberAnimation {duration: Constants.animation_duration}
	}

	QtObject {
		id: d

		property int activeModule
		readonly property int startupModule: SettingsModel.startupModule
		readonly property bool initialLockedAndHidden: startupModule === UiModule.IDENTIFY

		Component.onCompleted: show(startupModule, initialLockedAndHidden)
	}

	NavigationView {
		id: navigationView

		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
			leftMargin: plugin.safeAreaMargins.left
			rightMargin: plugin.safeAreaMargins.right
		}

		height: Style.dimens.navigation_bar_height

		Accessible.ignored: lockedAndHidden
	}
}
