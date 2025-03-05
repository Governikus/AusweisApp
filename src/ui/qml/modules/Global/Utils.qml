/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma Singleton

import QtQuick

import Governikus.Type

QtObject {
	function positionViewAtItem(pItem, pParent = pItem.parent) {
		if (pParent === null) {
			return;
		}
		if (typeof pParent.positionViewAtItem === "function") {
			pParent.positionViewAtItem(pItem);
			return;
		}
		positionViewAtItem(pItem, pParent.parent);
	}
	function shuffle(pArray) {
		for (let i = pArray.length - 1; i > 0; i--) {
			let j = ApplicationModel.randomInt(0, i);
			let x = pArray[i];
			pArray[i] = pArray[j];
			pArray[j] = x;
		}
		return pArray;
	}
}
