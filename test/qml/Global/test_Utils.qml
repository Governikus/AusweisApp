/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

import QtTest

import Governikus.Global

TestCase {
	id: testCase

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
