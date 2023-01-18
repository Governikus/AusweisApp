/*
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.ChangePinView 1.0; ChangePinView {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_ChangePinView"
	visible: true
	when: windowShown
}
