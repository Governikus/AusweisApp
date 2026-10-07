/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import Governikus.ResultView
			import Governikus.Type

			InputErrorView {
				title: \"test\"
				inputError: \"test\"
				passwordType: NumberModel.PasswordType.PIN
				returnCode: PaceResult.INVALID_PIN_1
			}
		", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_InputErrorView"
	visible: true
	when: windowShown
}
