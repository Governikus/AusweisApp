/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Style
import Governikus.Type

Item {
	id: root

	readonly property int activeModule: d.activeModule
	property bool lockedAndHidden: false
	required property real safeAreaBottomMargin

	signal resetContentArea

	function show(pModule) {
		if (d.activeModule !== pModule) {
			root.resetContentArea();
			d.activeModule = pModule;
			SettingsModel.startupModule = pModule === UiModule.REMOTE_SERVICE ? UiModule.REMOTE_SERVICE : UiModule.DEFAULT;
		}
	}

	enabled: !lockedAndHidden
	height: safeAreaBottomMargin + navigationView.implicitHeight

	states: State {
		when: root.lockedAndHidden

		PropertyChanges {
			root.height: 0
		}
	}
	transitions: Transition {
		enabled: !ApplicationModel.screenReaderRunning

		NumberAnimation {
			duration: Style.animation_duration
			property: "height"
			target: root
		}
	}

	QtObject {
		id: d

		property int activeModule
		readonly property bool initialLockedAndHidden: startupModule === UiModule.IDENTIFY || startupModule === UiModule.ONBOARDING
		readonly property int startupModule: SettingsModel.startupModule

		Component.onCompleted: root.show(startupModule)
	}
	ColumnLayout {
		anchors.left: parent.left
		anchors.right: parent.right

		NavigationView {
			id: navigationView

			Accessible.ignored: root.lockedAndHidden
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			activeModule: d.activeModule
			safeAreaBottomMargin: root.safeAreaBottomMargin
			visible: root.height > 0

			onShow: pModule => {
				root.show(pModule);
			}
		}
	}
}
