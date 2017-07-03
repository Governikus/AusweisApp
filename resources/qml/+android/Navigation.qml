import QtQuick 2.5
import QtQuick.Controls 2.0

import "global"

Item {
	id: navigation
	state: "identify"
	width: !PlatformConstants.is_tablet || lockedAndHidden ? 0 : Constants.menubar_width

	property bool lockedAndHidden: false
	property bool isOpen: false
	property int currentIndex: 0

	signal opened()
	signal closed()

	onLockedAndHiddenChanged: {
		if (lockedAndHidden) {
			drawer.close()
		}
	}

	onOpened: {
		isOpen = true
	}

	onClosed: {
		isOpen = false
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

		// FIXME:
		// when lockedAndHidden is true, dragging is still possible although the documentation stated:
		// "Setting the value to 0 or less prevents opening the drawer by dragging"
		dragMargin: lockedAndHidden ? 0 : Qt.styleHints.startDragDistance

		onOpened: {
			parent.opened()
		}

		onClosed: {
			parent.closed()
		}

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
