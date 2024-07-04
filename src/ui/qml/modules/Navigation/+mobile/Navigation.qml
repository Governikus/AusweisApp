/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type

RowLayout {
	id: navigation

	readonly property int activeModule: d.activeModule
	readonly property real effectiveHeight: UiPluginModel.safeAreaMargins.bottom + (lockedAndHidden ? 0 : implicitHeight)
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
	height: lockedAndHidden ? 0 : effectiveHeight
	visible: height > 0

	Behavior on height {
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
		Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
		Layout.fillWidth: true
		Layout.minimumHeight: Style.dimens.navigation_bar_min_height
		Layout.minimumWidth: Math.min(navigation.width, Style.dimens.max_text_width)
		Layout.preferredHeight: Math.ceil(implicitHeight)
	}
}
