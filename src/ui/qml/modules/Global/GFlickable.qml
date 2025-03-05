/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls

import Governikus.Global
import Governikus.Style

Flickable {
	id: root

	function handleKeyPress(key) {
		if (key === Qt.Key_PageDown || key === Qt.Key_Down)
			root.scrollPageDown();
		else if (key === Qt.Key_PageUp || key === Qt.Key_Up)
			root.scrollPageUp();
		else if (key === Qt.Key_End)
			root.contentY = root.contentHeight - root.height;
		else if (key === Qt.Key_Home)
			root.contentY = root.originY;
	}
	function highlightScrollbar() {
		if (ScrollBar.vertical)
			(ScrollBar.vertical as GScrollBar).highlight();
	}
	function positionViewAtBeginning() {
		contentY = originY;
	}
	function positionViewAtItem(pItem) {
		let item = pItem.parent ? pItem.parent : pItem;
		let mappedPosition = this.mapFromItem(item, pItem.x, pItem.y);
		if (mappedPosition.y < 0) {
			contentY = Math.max(0, contentY + mappedPosition.y);
			return;
		}
		let viewDifference = mappedPosition.y + pItem.height - height;
		if (viewDifference > 0) {
			contentY = Math.min(contentY + viewDifference, contentHeight - height);
		}
	}
	function scrollPageDown() {
		scrollBar.increase();
	}
	function scrollPageUp() {
		scrollBar.decrease();
	}

	Accessible.focusable: false
	Accessible.ignored: Style.is_layout_desktop
	Accessible.role: Accessible.ScrollBar
	boundsBehavior: Style.is_layout_desktop ? Flickable.StopAtBounds : (contentHeight <= height ? Flickable.StopAtBounds : Flickable.DragAndOvershootBounds)
	boundsMovement: Flickable.FollowBoundsBehavior
	flickDeceleration: Style.flickDeceleration
	flickableDirection: Flickable.VerticalFlick
	maximumFlickVelocity: Style.scrolling_speed

	ScrollBar.vertical: GScrollBar {
		id: scrollBar

		bottomPadding: Style.dimens.scrollbar_padding_vertical
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
