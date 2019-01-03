import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

Item {
	property int leftOverlayMargin: 0
	id: navigation
	width: !Constants.is_tablet || lockedAndHidden ? 0 : Constants.menubar_width
	enabled: !lockedAndHidden

	property bool lockedAndHidden: true
	property bool isOpen: drawer.position > 0
	property int currentIndex

	Component.onCompleted: {
		state = settingsModel.showTutorialOnStart ? "tutorial" : "identify"
		currentIndex = settingsModel.showTutorialOnStart ? 5 : 0
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
		color: Constants.background_color
		clip: true

		NavigationView {
			navigationController: navigation
		}

		Rectangle {
			id: iconBarBorder
			anchors.right: parent.right
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			width: Utils.dp(2)
			color: Constants.grey_border
		}
	}

	Drawer {
		id: drawer
		x: Constants.is_tablet ? Constants.menubar_width : 0
		y: Constants.titlebar_height
		height: navigation.height
		width:  Utils.dp(250)
		opacity: Constants.is_tablet ? 0 : 1
		dragMargin: lockedAndHidden ? 0 : Utils.dp(Qt.styleHints.startDragDistance)

		onPositionChanged: {
			if (Constants.is_tablet && position > 0) {
				navigation.leftOverlayMargin = Constants.menubar_width + (width - Constants.menubar_width) * position
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
