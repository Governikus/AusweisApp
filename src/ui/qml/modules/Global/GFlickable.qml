/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style

Flickable {
	id: baseItem

	function handleKeyPress(key) {
		if (key === Qt.Key_PageDown)
			baseItem.scrollPageDown();
		else if (key === Qt.Key_PageUp)
			baseItem.scrollPageUp();
		else if (key === Qt.Key_End)
			baseItem.contentY = baseItem.contentHeight - baseItem.height;
		else if (key === Qt.Key_Home)
			baseItem.contentY = baseItem.originY;
	}
	function highlightScrollbar() {
		if (ScrollBar.vertical)
			ScrollBar.vertical.highlight();
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
		Utils.scrollPageDown(baseItem);
	}
	function scrollPageUp() {
		Utils.scrollPageUp(baseItem);
	}

	Accessible.focusable: false
	Accessible.role: Accessible.ScrollBar
	boundsBehavior: Constants.is_desktop ? Flickable.StopAtBounds : (contentHeight <= height ? Flickable.StopAtBounds : Flickable.DragAndOvershootBounds)
	boundsMovement: Flickable.FollowBoundsBehavior
	flickDeceleration: Constants.flickDeceleration
	flickableDirection: Flickable.VerticalFlick
	maximumFlickVelocity: Constants.scrolling_speed

	ScrollBar.vertical: GScrollBar {
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
