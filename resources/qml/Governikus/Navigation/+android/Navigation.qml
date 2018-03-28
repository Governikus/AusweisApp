import QtQuick 2.5
import QtQuick.Controls 2.0

import Governikus.Global 1.0

Item {
	id: navigation
	state: "identify"
	width: !PlatformConstants.is_tablet || lockedAndHidden ? 0 : Constants.menubar_width
	enabled: !lockedAndHidden

	property bool lockedAndHidden: true
	property bool isOpen: drawer.position > 0
	property int currentIndex: 0

	onLockedAndHiddenChanged: {
		if (lockedAndHidden) {
			drawer.close()
		}
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
		visible: PlatformConstants.is_tablet
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.bottom: parent.bottom
		width: Math.max(parent.width, appWindow.leftOverlayMargin);
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
			color: PlatformConstants.grey_border
		}
	}

	Drawer {
		id: drawer
		topPadding: Constants.titlebar_height
		height: appWindow.height
		width:  Utils.dp(250)
		opacity: PlatformConstants.is_tablet ? 0 : 1
		background: Item {
			opacity: 0
			visible: !lockedAndHidden && drawer.position > 0

			MouseArea {
				anchors.fill: parent
				onClicked: drawer.close()
			}
		}

		dragMargin: lockedAndHidden ? 0 : Utils.dp(Qt.styleHints.startDragDistance)

		onPositionChanged: {
			if (PlatformConstants.is_tablet && position > 0) {
				appWindow.leftOverlayMargin = Constants.menubar_width + (width - Constants.menubar_width) * position
			} else {
				appWindow.leftOverlayMargin = 0
			}
		}

		contentItem: NavigationView {
			navigationController: navigation
		}
	}
}
