/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.ChangePinView; ChangePinController {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_ChangePinController"
	visible: true
	when: windowShown
}
