/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global
import Governikus.ResultView

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.ResultView; ResultView { title: \"ResultView\" }", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_ResultView"
	visible: true
	when: windowShown
}
