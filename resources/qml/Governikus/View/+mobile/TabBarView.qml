import QtQuick 2.10
import QtQuick.Controls 2.3
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
	property var pendingItems: new Array()

	QtObject {
		id: d

		property var pendingSignals: []

		function push(pSectionPage) {
			if (stack.currentItem === pSectionPage) {
				return
			}

			if (baseItem.pushed) {
				pSectionPage.firePush.connect(baseItem.push)
				pSectionPage.fireReplace.connect(baseItem.replace)
				pSectionPage.firePop.connect(baseItem.pop)
				pSectionPage.firePopAll.connect(baseItem.popAll)
				stack.push(pSectionPage)
				pSectionPage.pushed = true
			}
			// Main item has not been loaded yet, delay push.
			else {
				baseItem.pendingItems.push({ "item": pSectionPage })
			}
		}


		function replace(pSectionPage) {
			if (stack.currentItem === pSectionPage) {
				return
			}

			if (stack.depth <= 1) {
				d.push(pSectionPage)
				return
			}

			pSectionPage.pushed = false
			var item = stack.currentItem
			item.firePush.disconnect(baseItem.push)
			item.fireReplace.disconnect(baseItem.replace)
			item.firePop.disconnect(baseItem.pop)
			item.firePopAll.disconnect(baseItem.popAll)

			var page = stack.replace(pSectionPage)
			page.firePush.connect(baseItem.push)
			page.fireReplace.connect(baseItem.replace)
			page.firePop.connect(baseItem.pop)
			page.firePopAll.connect(baseItem.popAll)
		}


		function pop() {
			var sectionPage = stack.pop()
			sectionPage.pushed = false
			sectionPage.firePush.disconnect(baseItem.push)
			sectionPage.fireReplace.disconnect(baseItem.replace)
			sectionPage.firePop.disconnect(baseItem.pop)
			sectionPage.firePopAll.disconnect(baseItem.popAll)
		}


		function popAll() {
			while (stack.depth > 1) {
				d.pop()
			}
		}

		// Workaround for QTBUG-57267
		function handlePendingSignals() {
			while (pendingSignals.length > 0) {
				if (stack.busy) {
					pendingSignalsTimer.start()
					return
				}

				console.log("Executing queued stack action.")
				pendingSignals.shift()()
			}
		}
	}


	Loader {
		id: loader
		asynchronous: true
		onLoaded: {
			baseItem.pushed = true
			item.topLevelPage = true
			push(item)
			item.pushed = true

			pendingItems.reverse();
			while (pendingItems.length > 0) {
				var pendingItem = pendingItems.pop()
				d.push(pendingItem["item"], pendingItem["properties"])
			}
		}

		sourceComponent: parent.visible || baseItem.prefetch ? parent.sourceComponent : loader.sourceComponent
	}


	StackView {
		id: stack
		anchors.fill: parent
	}

	Timer {
		id: pendingSignalsTimer
		interval: 100
		repeat: false
		onTriggered: d.handlePendingSignals()
	}

	function push(pSectionPage) {
		d.pendingSignals.push(function() {d.push(pSectionPage)})
		d.handlePendingSignals()
	}


	function replace(pSectionPage) {
		d.pendingSignals.push(function() {d.replace(pSectionPage)})
		d.handlePendingSignals()
	}


	function pop() {
		d.pendingSignals.push(function() {d.pop()})
		d.handlePendingSignals()
	}


	function popAll() {
		d.pendingSignals.push(function() {d.popAll()})
		d.handlePendingSignals()
	}
}
