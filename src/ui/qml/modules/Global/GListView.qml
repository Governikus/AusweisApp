/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style

ListView {
	id: root

	property bool scrollBarAutohide: !Style.is_layout_desktop
	property real scrollBarBottomPadding: 0
	property alias scrollBarColor: scrollBar.color
	property real scrollBarTopPadding: 0

	function handleItemFocused(pIndex) {
		positionViewAtIndex(pIndex, ListView.Center);
		currentIndex = pIndex;
	}
	function handleKeyPress(event) {
		if (root.contentHeight <= root.height)
			return;

		let key = event.key;
		if (key === Qt.Key_PageDown) {
			if (root.atYEnd)
				return;
			root.scrollPageDown();
			root.setIndexToCurrentlyCenteredItem();
			event.accepted = true;
		} else if (key === Qt.Key_PageUp) {
			if (root.atYBeginning)
				return;
			root.scrollPageUp();
			root.setIndexToCurrentlyCenteredItem();
			event.accepted = true;
		} else if (key === Qt.Key_End) {
			root.positionViewAtEnd();
			root.currentIndex = count - 1;
			event.accepted = true;
		} else if (key === Qt.Key_Home) {
			root.positionViewAtBeginning();
			root.currentIndex = 0;
			event.accepted = true;
		}
	}
	function highlightScrollbar() {
		if (ScrollBar.vertical)
			(ScrollBar.vertical as GScrollBar).highlight();
	}
	function isListElementEmptyFunc(pItem) {
		return false;
	}
	function scrollPageDown() {
		scrollBar.increase();
	}
	function scrollPageUp() {
		scrollBar.decrease();
	}
	function setIndexToCurrentlyCenteredItem() {
		let index = indexAt(1, contentY + height / 2);
		if (index === -1)
			return;

		var item = itemAtIndex(index);
		while (isListElementEmptyFunc(item)) {
			index++;
			if (index === count - 1)
				return;
			item = itemAtIndex(index);
		}
		currentIndex = index;
	}

	Accessible.role: Accessible.List
	activeFocusOnTab: true
	boundsBehavior: Style.is_layout_desktop ? Flickable.StopAtBounds : (contentHeight <= height ? Flickable.StopAtBounds : Flickable.DragAndOvershootBounds)
	boundsMovement: Flickable.FollowBoundsBehavior
	flickDeceleration: Style.flickDeceleration
	flickableDirection: Flickable.VerticalFlick
	highlightMoveDuration: 0
	highlightResizeDuration: 0
	maximumFlickVelocity: Style.scrolling_speed

	ScrollBar.vertical: GScrollBar {
		id: scrollBar

		autohide: root.scrollBarAutohide
		bottomPadding: root.scrollBarBottomPadding + Style.dimens.scrollbar_padding_vertical
		topPadding: root.scrollBarTopPadding + Style.dimens.scrollbar_padding_vertical
	}

	Accessible.onDecreaseAction: scrollPageUp()
	Accessible.onIncreaseAction: scrollPageDown()
	Accessible.onScrollDownAction: scrollPageDown()
	Accessible.onScrollUpAction: scrollPageUp()
	Keys.onDownPressed: {
		do {
			root.incrementCurrentIndex();
		} while (isListElementEmptyFunc(currentItem) && root.currentIndex + 1 < root.count)
	}
	Keys.onPressed: event => {
		handleKeyPress(event);
	}
	Keys.onUpPressed: {
		do {
			root.decrementCurrentIndex();
		} while (isListElementEmptyFunc(currentItem) && root.currentIndex > 0)
	}
	onVisibleChanged: if (visible)
		highlightScrollbar()
}
