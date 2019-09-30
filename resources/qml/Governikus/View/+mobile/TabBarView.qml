/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.Style 1.0

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

		function disconnectSectionPageSignals(pSectionPage) {
			if (pSectionPage === null) {
				console.warn("tried to disconnect signals from null")
				return
			}

			pSectionPage.pushed = false
			pSectionPage.firePush.disconnect(baseItem.push)
			pSectionPage.firePushWithProperties.disconnect(baseItem.push)
			pSectionPage.fireReplace.disconnect(baseItem.replace)
			pSectionPage.firePop.disconnect(baseItem.pop)
			pSectionPage.firePopAll.disconnect(baseItem.popAll)
		}

		function connectSectionPageSignals(pSectionPage) {
			if (pSectionPage === null) {
				console.warn("tried to connect signals to null")
				return
			}

			pSectionPage.firePush.connect(baseItem.push)
			pSectionPage.firePushWithProperties.connect(baseItem.push)
			pSectionPage.fireReplace.connect(baseItem.replace)
			pSectionPage.firePop.connect(baseItem.pop)
			pSectionPage.firePopAll.connect(baseItem.popAll)
			pSectionPage.pushed = true
		}

		function push(pSectionPage, pProperties) {
			if (stack.currentItem === pSectionPage) {
				return
			}

			if (baseItem.pushed) {
				let item = stack.push(pSectionPage, pProperties)
				connectSectionPageSignals(item)
			}
			// Main item has not been loaded yet, delay push.
			else {
				baseItem.pendingItems.push({ "item": pSectionPage, "properties" : pProperties })
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
			item.firePushWithProperties.disconnect(baseItem.push)
			item.fireReplace.disconnect(baseItem.replace)
			item.firePop.disconnect(baseItem.pop)
			item.firePopAll.disconnect(baseItem.popAll)

			var page = stack.replace(pSectionPage)
			page.firePush.connect(baseItem.push)
			page.firePushWithProperties.connect(baseItem.push)
			page.fireReplace.connect(baseItem.replace)
			page.firePop.connect(baseItem.pop)
			page.firePopAll.connect(baseItem.popAll)
		}


		function pop() {
			var sectionPage = stack.pop()
			disconnectSectionPageSignals(sectionPage)
		}

		function popAll() {
			for (let i = stack.depth - 1; i > 0; i--) {
				disconnectSectionPageSignals(stack.get(i))
			}

			stack.pop(null)
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

	MouseArea {
		id: iosBackGestureMouseArea

		readonly property real minSwipeDistance: parent.width * 0.2
		readonly property real minVelocity: 10
		readonly property real touchStartAreaWidth: 10
		property real startPosX: 0.0
		property real previousPosX: 0.0
		property real velocity: 0.0

		anchors.fill: parent

		enabled: Constants.is_layout_ios
		preventStealing: true

		onPressed: {
			if (mouse.x < touchStartAreaWidth && currentSectionPage.navigationAction.state === "back") {
				mouse.accepted = true
				startPosX = mouse.x
				previousPosX = startPosX
				velocity = 0.0
			} else {
				mouse.accepted = false
			}
		}

		onPositionChanged: {
			let currentVelocity = mouse.x - previousPosX
			velocity = (velocity + currentVelocity) / 2.0
			previousPosX = mouse.x
		}

		onReleased: {
			let swipeDistance = mouse.x - startPosX
			if (swipeDistance > minSwipeDistance && velocity > minVelocity) {
				currentSectionPage.navigationAction.clicked()
			}
		}
	}

	Timer {
		id: pendingSignalsTimer
		interval: 100
		repeat: false
		onTriggered: d.handlePendingSignals()
	}

	function push(pSectionPage, pProperties) {
		d.pendingSignals.push(function() {d.push(pSectionPage, pProperties)})
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

	onVisibleChanged: {
		if (currentSectionPage) {
			currentSectionPage.onActivated()
		}
	}

	onCurrentSectionPageChanged: {
		if (currentSectionPage) {
			currentSectionPage.onActivated()
		}
	}
}
