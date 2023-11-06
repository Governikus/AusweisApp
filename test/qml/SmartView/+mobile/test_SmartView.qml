/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global
import Governikus.ResultView

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.SmartView; SmartView {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_SmartView"
	visible: true
	when: windowShown
}
