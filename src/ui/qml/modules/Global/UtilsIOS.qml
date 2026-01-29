/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

pragma Singleton

import QtQuick

QtObject {
	function findAllAccessibleItems(pItem) {
		let accessibleItems = [];

		function traverse(currentItem) {
			if (!currentItem || !currentItem.visible) {
				return;
			}

			const isA11yFocusable = currentItem.Accessible && currentItem.Accessible.focusable && !currentItem.Accessible.ignored;
			if (isA11yFocusable) {
				accessibleItems.push(currentItem);
			}

			for (let i = 0; i < currentItem.children.length; i++) {
				traverse(currentItem.children[i]);
			}
		}

		traverse(pItem);
		return accessibleItems;
	}
	function findNextAccessibleItemInFlickable(pFlickable, pCurrentItem) {
		const sortedItems = getSortedAccessibleItems(pFlickable);
		const currentIndex = sortedItems.indexOf(pCurrentItem);
		if (currentIndex !== -1 && currentIndex < sortedItems.length - 1) {
			return sortedItems[currentIndex + 1];
		}
		return null;
	}
	function findPreviousAccessibleItemInFlickable(pFlickable, pCurrentItem) {
		const sortedItems = getSortedAccessibleItems(pFlickable);
		const currentIndex = sortedItems.indexOf(pCurrentItem);
		if (currentIndex > 0) {
			return sortedItems[currentIndex - 1];
		}
		return null;
	}
	function getSortedAccessibleItems(pFlickable) {
		if (!pFlickable || !pFlickable.contentItem) {
			return [];
		}
		const allItems = findAllAccessibleItems(pFlickable.contentItem);
		return allItems.sort((a, b) => {
			const yA = a.mapToItem(pFlickable.contentItem, 0, 0).y;
			const yB = b.mapToItem(pFlickable.contentItem, 0, 0).y;
			return yA - yB;
		});
	}
	function handleFlickable(pFlickable, pItem) {
		let firstItemInGroup = findPreviousAccessibleItemInFlickable(pFlickable, pItem);
		firstItemInGroup = firstItemInGroup !== null ? firstItemInGroup : pItem;
		let lastItemInGroup = findNextAccessibleItemInFlickable(pFlickable, pItem);
		lastItemInGroup = lastItemInGroup !== null ? lastItemInGroup : pItem;

		const groupTopY = firstItemInGroup.mapToItem(pFlickable.contentItem, 0, 0).y;
		const groupBottomY = lastItemInGroup.mapToItem(pFlickable.contentItem, 0, 0).y + lastItemInGroup.height;

		const visibleTopY = pFlickable.contentY;
		const visibleBottomY = pFlickable.contentY + pFlickable.height;

		let newContentY = pFlickable.contentY;

		if (groupTopY < visibleTopY) {
			newContentY = groupTopY;
		} else if (groupBottomY > visibleBottomY) {
			newContentY = groupBottomY - pFlickable.height;
		}

		if (pFlickable.contentHeight > pFlickable.height) {
			pFlickable.contentY = Math.max(0, Math.min(newContentY, pFlickable.contentHeight - pFlickable.height));
		}
	}
}
