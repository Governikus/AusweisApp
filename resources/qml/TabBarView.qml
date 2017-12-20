import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import Governikus.Global 1.0

Item {
	id: baseItem

	property Component sourceComponent
	readonly property bool ready: loader.status === Loader.Ready
	property bool prefetch: false
	readonly property var currentSectionPage: stack.currentItem

	property alias stack: stack

	// Item provided by loader has been pushed
	property bool pushed: false

	// Items that should be pushed after the main item has been loaded and pushed
	property var pendingItems: []

	Loader {
		id: loader
		asynchronous: true
		onLoaded: {
			baseItem.pushed = true
			item.topLevelPage = true
			push(item)

			for (var i = 0; i < baseItem.pendingItems.length; i++) {
				var pendingItem = baseItem.pendingItems[i]
				push(pendingItem["item"], pendingItem["properties"])
			}

			baseItem.pendingItems = []
		}

		sourceComponent: parent.visible || baseItem.prefetch ? parent.sourceComponent : loader.sourceComponent
	}

	StackView {
		id: stack
		anchors.fill: parent
	}

	function push(sectionPage, properties) {
		if (baseItem.pushed) {
			sectionPage.firePush.connect(baseItem.push)
			sectionPage.firePop.connect(baseItem.pop)
			sectionPage.firePopAll.connect(baseItem.popAll)
			stack.push(sectionPage, properties)
		}
		// Main item has not been loaded yet, delay push.
		else {
			baseItem.pendingItems.push({ "item": sectionPage, "properties": properties })
		}
	}

	function pop() {
		var sectionPage = stack.pop()
		sectionPage.firePush.disconnect(baseItem.push)
		sectionPage.firePop.disconnect(baseItem.pop)
		sectionPage.firePopAll.disconnect(baseItem.popAll)
	}

	function popAll() {
		while (stack.depth > 1) {
			pop()
		}
	}
}
