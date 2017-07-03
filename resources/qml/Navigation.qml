import QtQuick 2.5

import "global"


Item {
	property bool lockedAndHidden: false
	property bool isOpen: true

	id: baseItem
	state: "identify"
	visible: !lockedAndHidden

	height: visible ? Constants.tabbar_height : 0


	Behavior on height {
		NumberAnimation {duration: 200}
	}


	function open() {
	}

	function close() {
	}

	NavigationView {}
}
