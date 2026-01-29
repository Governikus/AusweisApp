/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.Type

FlickableSectionPage {
	id: root

	enableTileStyle: false
	fillWidth: true
	margins: 0
	showTitleBarContent: false
	title: ""

	DelegateModel {
		id: tileModel

		delegate: Tile {
			id: tileDelegate

			required property url imagePath
			required property int index
			required property int module
			required property string titleText

			Accessible.name: titleText + ". " + qsTr("Item %1 of %2").arg(index + 1).arg(tileView.count) + (tileView.allItemsVisible ? "" : " . " + tileView.scrollHint)
			focusPolicy: ApplicationModel.screenReaderRunning ? Qt.StrongFocus : Qt.TabFocus
			height: ListView.view.height
			image: imagePath
			title: titleText
			width: tileView.itemWidth

			Accessible.onScrollLeftAction: if (tileView.isIos)
				tileView.decrementCurrentIndex()
			Accessible.onScrollRightAction: if (tileView.isIos)
				tileView.incrementCurrentIndex()
			Component.onCompleted: tileDelegate.DelegateModel.inItems = module !== UiModule.SMART_EID || ApplicationModel.smartSupported
			onClicked: root.show(module)
			onFocusChanged: if (focus && (!tileView.moving || ApplicationModel.screenReaderRunning))
				tileView.positionViewAtIndex(index, ListView.SnapPosition)

			FocusFrame {
				marginFactor: -2
			}
		}
		model: ListModel {
			ListElement {
				imagePath: "qrc:///images/mobile/device_tile.svg"
				module: UiModule.CHECK_ID_CARD
				//: MOBILE
				titleText: qsTr("Check device and ID card")
			}
			ListElement {
				imagePath: "qrc:///images/lock.svg"
				module: UiModule.PINMANAGEMENT
				//: MOBILE
				titleText: qsTr("Change PIN")
			}
			ListElement {
				imagePath: "qrc:///images/mydata_tile.svg"
				module: UiModule.SELF_AUTHENTICATION
				//: MOBILE
				titleText: qsTr("See my personal data")
			}
			ListElement {
				imagePath: "qrc:///images/mobile/smarteid.svg"
				module: UiModule.SMART_EID
				//: MOBILE
				titleText: qsTr("Smart-eID")
			}
		}
	}
	GSpacer {
		Layout.fillHeight: true
		Layout.minimumHeight: Style.dimens.pane_padding
	}
	GListView {
		id: tileView

		readonly property bool allItemsVisible: root.width > allItemsWidth
		readonly property int allItemsWidth: count * (itemWidth + spacing) - spacing
		readonly property bool isIos: Qt.platform.os === "ios"
		readonly property real itemWidth: Math.min(maximumItemWidth, Math.ceil(root.width * overlapFactor))
		property real maximumItemWidth: 1
		property real minItemHeight: 1
		property real minItemWidth: 1
		readonly property real overlapFactor: 0.72
		readonly property string scrollHint: isIos ? qsTr("Three finger swipe to scroll.") : qsTr("Two finger swipe to scroll.")

		function updateTileLimits() {
			var newMaximumItemWidth = -1;
			for (var index = 0; index < tileView.count; index++) {
				let item = tileView.itemAtIndex(index);
				if (!item)
					continue;
				if (index === 0) {
					tileView.minItemHeight = item.Layout.minimumHeight;
					tileView.minItemWidth = item.Layout.minimumWidth;
				}
				newMaximumItemWidth = Math.max(newMaximumItemWidth, item.implicitWidth);
			}
			if (newMaximumItemWidth >= 0) {
				tileView.maximumItemWidth = Math.ceil(newMaximumItemWidth);
			}
		}

		Layout.bottomMargin: Style.dimens.pane_spacing
		Layout.fillHeight: true
		Layout.fillWidth: true
		Layout.maximumHeight: 400
		Layout.maximumWidth: allItemsVisible ? allItemsWidth : Number.POSITIVE_INFINITY
		Layout.minimumHeight: minItemHeight
		Layout.minimumWidth: Math.ceil(minItemWidth / overlapFactor)
		boundsBehavior: Flickable.DragAndOvershootBounds
		cacheBuffer: Number.POSITIVE_INFINITY
		highlightMoveDuration: 250
		highlightRangeMode: allItemsVisible ? ListView.NoHighlightRange : ListView.StrictlyEnforceRange
		interactive: !allItemsVisible || UiPluginModel.isChromeOS
		maximumFlickVelocity: 4 * width
		model: tileModel
		orientation: Qt.Horizontal
		preferredHighlightBegin: width / 2 - itemWidth / 2
		preferredHighlightEnd: width / 2 + itemWidth / 2
		reuseItems: true
		snapMode: ListView.SnapOneItem

		Component.onCompleted: currentIndex = 1
		onCountChanged: updateTileLimits()
		onHeightChanged: updateTileLimits()
		onWidthChanged: updateTileLimits()
	}
	PageIndicator {
		id: indicator

		Layout.alignment: Qt.AlignCenter
		count: tileView.count
		currentIndex: tileView.currentIndex
		opacity: tileView.allItemsVisible ? 0 : 1

		delegate: Rectangle {
			required property int index

			color: index === indicator.currentIndex ? Style.color.control.border.basic_unchecked : Style.color.control.border.disabled_unchecked
			implicitHeight: Style.dimens.pageindicator_size
			implicitWidth: Style.dimens.pageindicator_size
			radius: width / 2
		}
	}
	GSpacer {
		Layout.fillHeight: true
		Layout.minimumHeight: Style.dimens.pane_padding
	}
}
