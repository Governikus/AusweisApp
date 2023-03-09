/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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

	signal resetContentArea

	function setLockedAndHidden(pLockedAndHidden = true) {
		d.lockedAndHidden = pLockedAndHidden;
	}
	function show(pModule, pLockedAndHidden = false) {
		setLockedAndHidden(pLockedAndHidden);
		if (d.activeModule !== pModule) {
			d.activeModule = pModule;
			SettingsModel.startupModule = pModule === UiModule.REMOTE_SERVICE ? UiModule.REMOTE_SERVICE : UiModule.DEFAULT;
		}
	}

	color: Style.color.background_pane
	enabled: !lockedAndHidden
	height: lockedAndHidden ? 0 : (Style.dimens.navigation_bar_height + plugin.safeAreaMargins.bottom)
	visible: height > 0

	Behavior on height  {
		id: heightAnimation
		enabled: !ApplicationModel.isScreenReaderRunning()

		NumberAnimation {
			duration: Constants.animation_duration
		}
	}

	QtObject {
		id: d

		property int activeModule
		readonly property bool initialLockedAndHidden: startupModule === UiModule.IDENTIFY
		property bool lockedAndHidden
		readonly property int startupModule: SettingsModel.startupModule

		Component.onCompleted: show(startupModule, initialLockedAndHidden)
	}
	NavigationView {
		id: navigationView
		Accessible.ignored: lockedAndHidden
		height: Style.dimens.navigation_bar_height

		anchors {
			left: parent.left
			leftMargin: plugin.safeAreaMargins.left
			right: parent.right
			rightMargin: plugin.safeAreaMargins.right
			top: parent.top
		}
	}
}
