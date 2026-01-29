/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls

import Governikus.Global
import Governikus.Style
import Governikus.Type

Flickable {
	id: root

	function findFocusableChild(pChildren, pScreenReaderRunning) {
		const isItemVisible = pItem => {
			if (!pItem.visible || !pItem.height) {
				return false;
			}

			const itemTop = pItem.mapToItem(contentItem, 0, 0).y;
			const itemBottom = itemTop + pItem.height;
			const viewTop = contentY;
			const viewBottom = viewTop + height;

			return itemTop >= viewTop && itemBottom <= viewBottom;
		};

		const isFocusable = (pItem, pScreenReaderRunning) => {
			if (!pItem) {
				return false;
			}

			if (pScreenReaderRunning) {
				return pItem.Accessible && pItem.Accessible.focusable && !pItem.Accessible.ignored;
			}

			return pItem.activeFocusOnTab;
		};

		for (const child of pChildren) {
			if (isItemVisible(child) && isFocusable(child, pScreenReaderRunning)) {
				return child;
			}
			if (child.children) {
				const nested = findFocusableChild(child.children, pScreenReaderRunning);
				if (nested) {
					return nested;
				}
			}
		}
		return null;
	}
	function focusElementAfterScroll() {
		const screenReaderRunning = ApplicationModel.screenReaderRunning;
		const item = findFocusableChild(contentItem.children, screenReaderRunning);
		item?.forceActiveFocus(screenReaderRunning ? Qt.MouseFocusReason : Qt.TabFocusReason);
	}
	function handleKeyPress(event) {
		switch (event.key) {
		case Qt.Key_Down:
			if (ApplicationModel.screenReaderRunning)
				return;
		// fall through
		case Qt.Key_PageDown:
			root.scrollPageDown();
			break;
		case Qt.Key_Up:
			if (ApplicationModel.screenReaderRunning)
				return;
		// fall through
		case Qt.Key_PageUp:
			root.scrollPageUp();
			break;
		case Qt.Key_End:
			root.contentY = root.contentHeight - root.height;
			break;
		case Qt.Key_Home:
			root.contentY = root.originY;
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
	function positionViewAtBeginning() {
		contentY = originY;
	}
	function scrollPageDown() {
		scrollBar.increase();
		focusElementAfterScroll();
	}
	function scrollPageUp() {
		scrollBar.decrease();
		focusElementAfterScroll();
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
		handleKeyPress(event);
	}
	onVisibleChanged: if (visible)
		highlightScrollbar()
}
