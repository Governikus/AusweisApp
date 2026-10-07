/**
 * Copyright (c) 2020-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.LogView; LogView {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_LogView"
	visible: true
	when: windowShown
}
