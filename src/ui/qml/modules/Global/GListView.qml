/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
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
	function handleKeyPress(key) {
		if (key === Qt.Key_PageDown)
			root.scrollPageDown();
		else if (key === Qt.Key_PageUp)
			root.scrollPageUp();
		else if (key === Qt.Key_End)
			root.positionViewAtEnd();
		else if (key === Qt.Key_Home)
			root.positionViewAtBeginning();
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

	Accessible.ignored: true
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
	Keys.onPressed: event => {
		handleKeyPress(event.key);
	}
	onVisibleChanged: if (visible)
		highlightScrollbar()
}
