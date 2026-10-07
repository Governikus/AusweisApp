/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

import QtTest

import Governikus.Global

TestCase {
	id: testCase

	function test_isAccessibleIgnored(data) {
		var item = null;
		if (data.setA11yIgnoredOnItem) {
			item = createTemporaryQmlObject("
				import QtQuick
				Item { Accessible.ignored: %1 }
		".arg(data.a11yIgnoredValue), testCase);
		} else {
			item = createTemporaryQmlObject("
				import QtQuick
				Item {}
			", testCase);
		}
		if (typeof item.hasAccessibleAttached === "function")
			compare(Utils.isAccessibleIgnored(item), data.expectedUtilslResult, "A11y-Ignored state mismatch (with a11y check patch)");
		else
			compare(Utils.isAccessibleIgnored(item), data.expectedUtilsResultWithoutGovernikusLibs, "A11y-Ignored state mismatch (no patch)");
	}
	function test_isAccessibleIgnored_data() {
		return [
			{
				"setA11yIgnoredOnItem": true,
				"a11yIgnoredValue": "true",
				"expectedUtilslResult": true,
				"expectedUtilsResultWithoutGovernikusLibs": true
			},
			{
				"setA11yIgnoredOnItem": true,
				"a11yIgnoredValue": "false",
				"expectedUtilslResult": false,
				"expectedUtilsResultWithoutGovernikusLibs": false
			},
			{
				"setA11yIgnoredOnItem": false,
				"a11yIgnoredValue": "",
				"expectedUtilslResult": true,
				"expectedUtilsResultWithoutGovernikusLibs": false // QTBUG-144607
			}
		];
	}
	function test_shuffle() {
		let numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0];
		let shuffledNumbers = Utils.shuffle([1, 2, 3, 4, 5, 6, 7, 8, 9, 0]);
		compare(shuffledNumbers.count, numbers.count);
		let isSameOrder = numbers.toString() === shuffledNumbers.toString();
		compare(isSameOrder, false, "Shuffled arrays have same order");
	}

	name: "test_Utils"
	visible: true
	when: windowShown
}
