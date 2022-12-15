/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQml.Models 2.15
import QtTest 1.15
import Governikus.Global 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; GRepeater {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_model() {
		expected.text = testModel.get(2).name;
		compare(testObject1.maxItemWidth, expected.implicitWidth, "maxItemWidth");
		testModel.append({
				"name": "Test"
			});
		compare(testObject1.maxItemWidth, expected.implicitWidth, "maxItemWidth");
		testModel.append({
				"name": "a large new text"
			});
		expected.text = testModel.get(4).name;
		compare(testObject1.maxItemWidth, expected.implicitWidth, "maxItemWidth");
		testModel.append({
				"name": "Test"
			});
		compare(testObject1.maxItemWidth, expected.implicitWidth, "maxItemWidth");
	}

	name: "test_GRepeater"
	visible: true
	when: windowShown

	Text {
		id: expected
	}
	ListModel {
		id: testModel
		ListElement {
			name: "Apple"
		}
		ListElement {
			name: "Orange"
		}
		ListElement {
			name: "Banana"
		}
	}
	GRepeater {
		id: testObject1
		model: testModel

		Text {
			text: name
		}
	}
}
