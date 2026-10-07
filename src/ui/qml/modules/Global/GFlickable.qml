/**
 * Copyright (c) 2019-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Controls

import Governikus.Global
import Governikus.Style
import Governikus.Type

Flickable {
	id: root

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
		const activeFocusItem = Window.activeFocusItem;
		scrollBar.increase();
		d.moveFocusAfterScroll(activeFocusItem, true);
	}
	function scrollPageUp() {
		const activeFocusItem = Window.activeFocusItem;
		scrollBar.decrease();
		d.moveFocusAfterScroll(activeFocusItem, false);
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

	QtObject {
		id: d

		function collectReachableFocusTargets(pItemsToSearch, pScreenReaderRunning, pFocusTargets) {
			let focusTargets = pFocusTargets || [];
			for (const item of pItemsToSearch) {
				if (isItemVisible(item) && isItemFocusable(item, pScreenReaderRunning)) {
					focusTargets.push(item);
				}
				if (item.visibleChildren) {
					collectReachableFocusTargets(item.visibleChildren, pScreenReaderRunning, focusTargets);
				}
			}
			return focusTargets;
		}
		function findAdjacentFocusTarget(pFocusTargets, pCurrentFocusItem, pForward) {
			for (let i = 0; i < pFocusTargets.length; i++) {
				if (pFocusTargets[i] === pCurrentFocusItem) {
					if (pForward && i === pFocusTargets.length - 1) {
						return pFocusTargets[i];
					}
					if (!pForward && i === 0) {
						return pFocusTargets[0];
					}
					return pForward ? pFocusTargets[i + 1] : pFocusTargets[i - 1];
				}
			}
			// item not in list means it is hidden by scrolling
			return pForward ? pFocusTargets[0] : pFocusTargets[pFocusTargets.length - 1];
		}
		function isItemFocusable(pItem, pScreenReaderRunning) {
			if (!pItem) {
				return false;
			}
			if (pScreenReaderRunning) {
				return !Utils.isAccessibleIgnored(pItem) && pItem.Accessible.focusable;
			}
			return pItem.activeFocusOnTab;
		}
		function isItemVisible(pItem) {
			if (!pItem.visible || !pItem.height) {
				return false;
			}
			const itemTop = pItem.mapToItem(root.contentItem, 0, 0).y;
			const itemBottom = itemTop + pItem.height;
			const viewTop = root.contentY;
			const viewBottom = viewTop + root.height;
			return itemTop >= viewTop && itemBottom <= viewBottom;
		}
		function moveFocusAfterScroll(pCurrentFocusItem, pForward) {
			const screenReaderRunning = ApplicationModel.screenReaderRunning;
			let focusTargets = d.collectReachableFocusTargets(root.contentItem.visibleChildren, screenReaderRunning);
			let item = d.findAdjacentFocusTarget(focusTargets, pCurrentFocusItem, pForward);
			item?.forceActiveFocus(screenReaderRunning ? Qt.MouseFocusReason : Qt.TabFocusReason);
		}
	}
}
