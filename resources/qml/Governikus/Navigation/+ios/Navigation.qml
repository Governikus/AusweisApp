import QtQuick 2.5

import Governikus.Global 1.0


Item {
	property bool lockedAndHidden: false
	property bool isOpen: true

	id: baseItem
	state: "identify"

	height: childrenRect.height
	Behavior on height {
		NumberAnimation {duration: 200}
	}


	function open() {
	}

	function close() {
	}

	NavigationView {
		visible: !baseItem.lockedAndHidden
		height: visible ? Constants.tabbar_height : 0
	}
}
