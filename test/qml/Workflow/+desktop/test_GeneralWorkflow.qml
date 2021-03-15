/*
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0
import Governikus.Type.ReaderPlugIn 1.0

TestCase {
	id: testCase

	name: "test_GeneralWorkflow"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Workflow 1.0; GeneralWorkflow {}", testCase)
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}
}
