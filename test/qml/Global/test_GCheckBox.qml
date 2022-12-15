/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Global 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; GCheckBox {}", testCase);
	}
	function test_checked() {
		let testObject = createTestObject();
		verify(!testObject.checked, "Initial checked: false");
		testObject.checked = true;
		verify(testObject.checked, "checked: true");
		testObject.checked = false;
		verify(!testObject.checked, "checked: false");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_text() {
		let testObject = createTestObject();
		testObject.text = "testText";
		verify(testObject.text === "testText", "Object text can be set");
	}

	name: "test_GCheckBox"
	visible: true
	when: windowShown

	GCheckBox {
		id: testObject
		TestCase {
			function test_click() {
				testObject.checked = false;
				verify(!testObject.checked, "Initial checked: false");
				mouseClick(testObject);
				tryVerify(function () {
						return testObject.checked;
					});
			}

			when: windowShown
		}
	}
}
