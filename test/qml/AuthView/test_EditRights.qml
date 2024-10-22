/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import Governikus.AuthView

			EditRights {
			}
		", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		let transactionText = findChild(testObject, "transactionText");
		verify(transactionText, "Child found");

		// Text.StyledText will show HTML-Escaped text again
		compare(transactionText.textFormat, Text.StyledText);
	}

	name: "test_EditRights"
	visible: true
	when: windowShown
}
