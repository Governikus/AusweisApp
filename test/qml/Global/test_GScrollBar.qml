/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtTest 1.15

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_GScrollBar"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; GScrollBar {}", testCase)
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}

	function test_highlight() {
		let testObject = createTestObject()

		tryVerify(function(){ return !testObject.highlighted })
		testObject.highlight()
		verify(testObject.highlighted, "highlighted: true")
		tryVerify(function(){ return !testObject.highlighted })
	}

	function test_click() {
		let testObject = createTestObject()

		mousePress(testObject)
		tryVerify(function(){ return testObject.pressed })
		mouseRelease(testObject)
		tryVerify(function(){ return !testObject.pressed })
	}

}
