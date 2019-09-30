/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	property int leftOverlayMargin: 0
	id: navigation
	width: !Constants.is_tablet || lockedAndHidden ? 0 : Style.dimens.menubar_width
	enabled: !lockedAndHidden

	property bool lockedAndHidden: true
	property bool isOpen: drawer.position > 0
	property int currentIndex

	signal reselectedState

	Component.onCompleted: {
		state = SettingsModel.showSetupAssistantOnStart ? "tutorial" : "identify"
		currentIndex = SettingsModel.showSetupAssistantOnStart ? 5 : 0
	}

	onLockedAndHiddenChanged: {
		if (lockedAndHidden) {
			drawer.close()
		}
	}

	onLeftOverlayMarginChanged: {
		Overlay.overlay.x = leftOverlayMargin
	}

	function open() {
		if (!lockedAndHidden) {
			drawer.open()
		}
	}

	function close() {
		drawer.close()
	}

	Rectangle {
		visible: Constants.is_tablet
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.bottom: parent.bottom
		width: Math.max(parent.width, navigation.leftOverlayMargin);
		color: Style.color.background
		clip: true

		NavigationView {
			navigationController: navigation
		}

		GSeparator {
			id: iconBarBorder
			anchors.right: parent.right
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			orientation: Qt.Vertical
			color: Style.color.border_dark
		}
	}

	Drawer {
		id: drawer
		x: Constants.is_tablet ? Style.dimens.menubar_width : 0
		y: Style.dimens.titlebar_height + plugin.safeAreaMargins.top

		height: navigation.height
		width: 250
		opacity: Constants.is_tablet ? 0 : 1
		dragMargin: lockedAndHidden ? 0 : Qt.styleHints.startDragDistance

		onPositionChanged: {
			if (Constants.is_tablet && position > 0) {
				navigation.leftOverlayMargin = Style.dimens.menubar_width + (width - Style.dimens.menubar_width) * position
			} else {
				navigation.leftOverlayMargin = 0
			}
		}

		contentItem: NavigationView {
			navigationController: navigation

			MouseArea {
				enabled: Constants.is_tablet
				anchors.fill: parent
				onClicked: drawer.close()
			}
		}
	}
}
