/**
 * Copyright (c) 2020-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GPaneBackground {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_GPaneBackground"
	visible: true
	when: windowShown
}
