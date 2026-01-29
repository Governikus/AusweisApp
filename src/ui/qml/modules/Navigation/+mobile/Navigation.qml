/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Style
import Governikus.Type

Item {
	id: root

	readonly property int activeModule: d.activeModule
	readonly property bool lockedAndHidden: d.lockedAndHidden
	required property real safeAreaBottomMargin

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
	height: safeAreaBottomMargin + navigationView.implicitHeight

	states: State {
		when: d.lockedAndHidden

		PropertyChanges {
			root.height: root.safeAreaBottomMargin
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
		property bool lockedAndHidden
		readonly property int startupModule: SettingsModel.startupModule

		Component.onCompleted: root.show(startupModule, initialLockedAndHidden)
	}
	ColumnLayout {
		anchors.left: parent.left
		anchors.right: parent.right

		NavigationView {
			id: navigationView

			Accessible.ignored: d.lockedAndHidden
			Layout.alignment: Qt.AlignHCenter
			activeModule: d.activeModule
			visible: root.height > root.safeAreaBottomMargin

			onShow: pModule => {
				root.resetContentArea();
				root.show(pModule);
			}
		}
	}
	Rectangle {
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		color: d.lockedAndHidden ? Style.color.background : Style.color.pane.background.basic_unchecked
		height: root.safeAreaBottomMargin
	}
}
