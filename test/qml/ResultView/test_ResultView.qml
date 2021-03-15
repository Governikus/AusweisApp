/*
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0
import Governikus.ResultView 1.0

TestCase {
	id: testCase

	name: "test_ResultView"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.ResultView 1.0; ResultView {}", testCase)
	}

	TestCase {
		when: testCase.completed
		optional: true

		function benchmark_load() {
			let testObject = createTestObject()
			testObject.destroy()
		}
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}

	function test_resultType() {
		let testObject = createTestObject()

		compare(testObject.resultType, ResultView.Type.IsSuccess, "Initial resultType: IsSuccess")

		testObject.resultType = ResultView.Type.IsError
		compare(testObject.resultType, ResultView.Type.IsError, "resultType: IsError")

		testObject.resultType = ResultView.Type.IsInfo
		compare(testObject.resultType, ResultView.Type.IsInfo, "resultType: IsInfo")
	}
}
