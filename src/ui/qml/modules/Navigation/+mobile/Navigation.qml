/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Style
import Governikus.Type

RowLayout {
	id: root

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

	enabled: !lockedAndHidden
	height: lockedAndHidden ? 0 : (UiPluginModel.safeAreaMargins.bottom + implicitHeight)
	visible: height > 0

	Behavior on height {
		id: heightAnimation

		enabled: !ApplicationModel.isScreenReaderRunning

		NumberAnimation {
			duration: Style.animation_duration
		}
	}

	QtObject {
		id: d

		property int activeModule
		readonly property bool initialLockedAndHidden: startupModule === UiModule.IDENTIFY || startupModule === UiModule.ONBOARDING
		property bool lockedAndHidden
		readonly property int startupModule: SettingsModel.startupModule

		Component.onCompleted: root.show(startupModule, initialLockedAndHidden)
	}
	NavigationView {
		id: navigationView

		Accessible.ignored: d.lockedAndHidden
		Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
		activeModule: d.activeModule

		onShow: pModule => {
			root.resetContentArea();
			root.show(pModule);
		}
	}
}
