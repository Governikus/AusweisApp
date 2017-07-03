import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import "global"

Item {
	id: baseItem

	property string source

	property alias stack: stack

	// Item provided by loader has been pushed
	property bool pushed: false

	// Items that should be pushed after the main item has been loaded and pushed
	property var pendingItems: []

	Loader {
		id: loader
		asynchronous: true
		onLoaded: {
			appWindow.hideSplashScreen()

			baseItem.pushed = true
			push(item)

			for (var i = 0; i < baseItem.pendingItems.length; i++) {
				var pendingItem = baseItem.pendingItems[i]
				push(pendingItem["item"], pendingItem["properties"])
			}

			baseItem.pendingItems = []
			numberModel.continueWorkflow()
		}

		source: parent.visible ? parent.source : loader.source
	}

	onVisibleChanged: {
		if (visible) {
			titleBar.pushTabBarSubView(stack.currentItem)
		}
	}

	StackView {
		id: stack
		anchors.fill: parent

		onCurrentItemChanged: {
			titleBar.pushTabBarSubView(currentItem)
		}
	}

	function push(sectionPage, properties) {
		if (baseItem.pushed) {
			stack.push(sectionPage, properties)
		}
		// Main item has not been loaded yet, delay push.
		else {
			baseItem.pendingItems.push({ "item": sectionPage, "properties": properties })
		}
	}

	function pop(item) {
		stack.pop(item)
	}
}
