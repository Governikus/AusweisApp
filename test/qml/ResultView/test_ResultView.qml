/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtTest 1.15

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
