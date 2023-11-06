/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
pragma Singleton
import QtQuick
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel

QtObject {
	function escapeHtml(pStr) {
		return String(pStr).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
	}
	function isSameDate(pOne, pAnother) {
		return pOne.getFullYear() === pAnother.getFullYear() && pOne.getMonth() === pAnother.getMonth() && pOne.getDate() === pAnother.getDate();
	}
	function isThisWeek(pDate) {
		let monday = new Date;
		monday.setDate(monday.getDate() - monday.getDay());
		pDate.setDate(pDate.getDate() - pDate.getDay());
		return isSameDate(monday, pDate);
	}
	function isToday(pDate) {
		let today = new Date;
		return isSameDate(today, pDate);
	}
	function isYesterday(pDate) {
		let yesterday = new Date;
		yesterday.setDate(yesterday.getDate() - 1);
		return isSameDate(yesterday, pDate);
	}
	function scrollPageDown(pFlickable) {
		if (pFlickable.height >= pFlickable.contentHeight) {
			return;
		}
		if (pFlickable.atYEnd) {
			return;
		}
		pFlickable.contentY = Math.min(pFlickable.contentY + pFlickable.height, pFlickable.originY + pFlickable.contentHeight - pFlickable.height);
	}
	function scrollPageUp(pFlickable) {
		if (pFlickable.height >= pFlickable.contentHeight) {
			return;
		}
		if (pFlickable.atYBeginning) {
			return;
		}
		pFlickable.contentY = Math.max(pFlickable.contentY - pFlickable.height, pFlickable.originY);
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
