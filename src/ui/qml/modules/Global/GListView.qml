/**
 * Copyright (c) 2019-2026 Governikus Service GmbH, Germany
 */

import Governikus.Global
import Governikus.Style
import Governikus.Type
import QtQuick
import QtQuick.Controls

ListView {
	id: root

	property bool enableLineShineThroughFix: false
	property bool keyNavigationChangesCurrentIndex: true
	property bool scrollBarAutohide: !Style.is_layout_desktop
	property real scrollBarBottomPadding: 0
	property alias scrollBarColor: scrollBar.color
	property bool scrollBarFocusFrame: false
	property real scrollBarTopPadding: 0

	function centerViewAtIndex(pIndex) {
		positionViewAtIndex(pIndex, ListView.Center);
		currentIndex = pIndex;
	}
	function handleKeyPress(event) {
		if (keyNavigationChangesCurrentIndex || ApplicationModel.screenReaderRunning) {
			switch (event.key) {
			case Qt.Key_Right:
			case Qt.Key_Down:
				do {
					root.incrementCurrentIndex();
				} while (!currentItem.enabled && root.currentIndex + 1 < root.count)
				highlightScrollbar();
				event.accepted = true;
				return;
			case Qt.Key_Left:
			case Qt.Key_Up:
				do {
					root.decrementCurrentIndex();
				} while (!currentItem.enabled && root.currentIndex > 0)
				highlightScrollbar();
				event.accepted = true;
				return;
			default:
				break;
			}
		}

		if (root.contentHeight <= root.height)
			return;

		switch (event.key) {
		case Qt.Key_Down:
		case Qt.Key_PageDown:
			if (root.atYEnd)
				return;
			root.scrollPageDown();
			root.setIndexToCurrentlyCenteredItem();
			highlightScrollbar();
			break;
		case Qt.Key_Up:
		case Qt.Key_PageUp:
			if (root.atYBeginning)
				return;
			root.scrollPageUp();
			root.setIndexToCurrentlyCenteredItem();
			highlightScrollbar();
			break;
		case Qt.Key_End:
			root.positionViewAtEnd();
			root.currentIndex = count - 1;
			highlightScrollbar();
			break;
		case Qt.Key_Home:
			root.positionViewAtBeginning();
			root.currentIndex = 0;
			highlightScrollbar();
			break;
		default:
			return;
		}
		event.accepted = true;
	}
	function highlightScrollbar() {
		if (ScrollBar.vertical)
			(ScrollBar.vertical as GScrollBar).highlight();
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
		while (!item.enabled) {
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
	keyNavigationEnabled: false
	maximumFlickVelocity: Style.scrolling_speed

	ScrollBar.vertical: GScrollBar {
		id: scrollBar

		autohide: root.scrollBarAutohide
		bottomPadding: root.scrollBarBottomPadding + Style.dimens.scrollbar_padding_vertical
		focusFrameScope: root.scrollBarFocusFrame ? root : null
		topPadding: root.scrollBarTopPadding + Style.dimens.scrollbar_padding_vertical
	}

	Accessible.onDecreaseAction: scrollPageUp()
	Accessible.onIncreaseAction: scrollPageDown()
	Accessible.onScrollDownAction: scrollPageDown()
	Accessible.onScrollUpAction: scrollPageUp()
	Keys.onPressed: event => {
		handleKeyPress(event);
	}
	onVisibleChanged: if (visible)
		highlightScrollbar()

	RoundedRectangle {
		// Fix to prevent background shine through during scroll animation: QTBUG-152056
		height: root.height
		visible: root.enableLineShineThroughFix
		width: root.width - Style.dimens.pane_padding
		z: -1
	}
}
