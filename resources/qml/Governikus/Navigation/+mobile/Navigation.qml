/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.UiModule 1.0

Rectangle {
	id: baseItem

	readonly property int activeModule: d.activeModule
	readonly property bool lockedAndHidden: d.lockedAndHidden

	signal resetContentArea()

	function show(pModule, pLockedAndHidden = false) {
		setLockedAndHidden(pLockedAndHidden)
		if (d.activeModule !== pModule) {
			d.activeModule = pModule

			SettingsModel.startupModule = pModule === UiModule.REMOTE_SERVICE ? UiModule.REMOTE_SERVICE : UiModule.DEFAULT
		}
	}

	function setLockedAndHidden(pLockedAndHidden = true) {
		d.lockedAndHidden = pLockedAndHidden
	}

	enabled: !lockedAndHidden
	visible: height > 0
	height: lockedAndHidden ? 0 : (Style.dimens.navigation_bar_height + plugin.safeAreaMargins.bottom)

	color: Style.color.background_pane

	Behavior on height {
		id: heightAnimation

		enabled: !ApplicationModel.isScreenReaderRunning()

		NumberAnimation {duration: Constants.animation_duration}
	}

	QtObject {
		id: d

		property int activeModule
		property bool lockedAndHidden
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
