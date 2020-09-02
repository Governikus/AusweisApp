/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

pragma Singleton

import QtQuick 2.10

import Governikus.Type.Random 1.0

QtObject {

	function escapeHtml(str)
	{
		return String(str).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
	}

	function isToday(date)
	{
		var today = new Date;
		return isSameDate(today, date);
	}

	function isYesterday(date)
	{
		var yesterday = new Date;
		yesterday.setDate(yesterday.getDate() - 1);
		return isSameDate(yesterday, date);
	}

	function isThisWeek(date)
	{
		var monday = new Date;
		monday.setDate(monday.getDate()-monday.getDay());

		date.setDate(date.getDate()-date.getDay());

		return isSameDate(monday, date);
	}

	function isSameDate(one, another)
	{
		return one.getFullYear() === another.getFullYear() && one.getMonth() === another.getMonth() && one.getDate() === another.getDate();
	}

	function providerIconSource(baseName) {
		var platform = plugin.platformStyle.indexOf("tablet") !== -1 ? "+tablet/" : ""

		return "qrc:///images/provider/" + platform + baseName + ".png"
	}

	function scrollPageDown(pFlickable) {
		if (pFlickable.height >= pFlickable.contentHeight) {
			return
		}

		if (pFlickable.atYEnd) {
			return
		}

		pFlickable.contentY = Math.min(pFlickable.contentY + pFlickable.height, pFlickable.originY + pFlickable.contentHeight - pFlickable.height)
	}

	function scrollPageUp(pFlickable) {
		if (pFlickable.height >= pFlickable.contentHeight) {
			return
		}

		if (pFlickable.atYBeginning) {
			return
		}

		pFlickable.contentY = Math.max(pFlickable.contentY - pFlickable.height, pFlickable.originY)
	}

	function shuffle(pArray) {
		for (var i = pArray.length - 1; i > 0; i--) {
			let j = Random.randomInt(0, i);
			let x = pArray[i];
			pArray[i] = pArray[j];
			pArray[j] = x;
		}
		return pArray;
	}

	function helpTopicOf(pComponent, pDefaultHelpTopic) {
		 if (pComponent && typeof(pComponent.helpTopic) !== "undefined") {
			return pComponent.helpTopic
		 } else {
			return pDefaultHelpTopic
		 }
	}
}
