/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import QtQuick

Repeater {
	id: root

	property bool autoConsiderItems: true
	property list<Item> consideredItems
	readonly property real maxItemWidth: {
		var maxWidth = 0;
		for (let i in consideredItems) {
			maxWidth = Math.max(maxWidth, consideredItems[i].implicitWidth);
		}
		return maxWidth;
	}

	function addConsideredItem(pItem) {
		consideredItems.push(pItem);
	}
	function removeConsideredItem(pItemToRemove) {
		var newList = [];
		for (let i in consideredItems) {
			if (consideredItems[i] !== pItemToRemove) {
				newList.push(consideredItems[i]);
			}
		}
		consideredItems = newList;
	}

	onItemAdded: (pIndex, pItem) => {
		if (autoConsiderItems) {
			addConsideredItem(pItem);
		}
	}
	onItemRemoved: (pIndex, pItemToRemove) => {
		removeConsideredItem(pItemToRemove);
	}
}
