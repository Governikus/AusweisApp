/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
pragma Singleton
import QtQuick 2.15
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

QtObject {
	function escapeHtml(pStr) {
		return String(pStr).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
	}
	function helpTopicOf(pComponent, pDefaultHelpTopic) {
		if (pComponent && typeof (pComponent.helpTopic) !== "undefined") {
			return pComponent.helpTopic;
		} else {
			return pDefaultHelpTopic;
		}
	}
	function historyDateString(pDate) {
		//: LABEL ALL_PLATFORMS
		return (isToday(pDate) ? qsTr("today") :
			//: LABEL ALL_PLATFORMS
			isYesterday(pDate) ? qsTr("yesterday") :
			// dddd is without translation because we want the long day name with every language
			isThisWeek(pDate) ? pDate.toLocaleString(Qt.locale(SettingsModel.language), "dddd") :
			//: LABEL ALL_PLATFORMS Date format according to https://doc.qt.io/qt/qdate.html#toString
			pDate.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy")));
	}
	function isSameDate(pOne, pAnother) {
		return pOne.getFullYear() === pAnother.getFullYear() && pOne.getMonth() === pAnother.getMonth() && pOne.getDate() === pAnother.getDate();
	}
	function isThisWeek(pDate) {
		var monday = new Date;
		monday.setDate(monday.getDate() - monday.getDay());
		pDate.setDate(pDate.getDate() - pDate.getDay());
		return isSameDate(monday, pDate);
	}
	function isToday(pDate) {
		var today = new Date;
		return isSameDate(today, pDate);
	}
	function isYesterday(pDate) {
		var yesterday = new Date;
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
