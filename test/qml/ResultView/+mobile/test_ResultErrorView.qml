/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtTest 1.15

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_ResultErrorView"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.ResultView 1.0; ResultErrorView {}", testCase)
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}

	function test_hasErrorDetails() {
		let testObject = createTestObject()
		compare(testObject.hasErrorDetails, false)
		testObject.errorCode = "SomeErrorCode"
		compare(testObject.hasErrorDetails, true)
	}
}
