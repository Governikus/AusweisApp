/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQml.Models
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GRepeater {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_model() {
		let testModel = testModelComponent.createObject(testCase);
		let testObject = testObjectComponent.createObject(testCase, {
			"model": testModel
		});
		expected.text = testModel.get(2).name;
		compare(testObject.maxItemWidth, expected.implicitWidth, "maxItemWidth");
		testModel.append({
			"name": "Test"
		});
		compare(testObject.maxItemWidth, expected.implicitWidth, "maxItemWidth");
		testModel.append({
			"name": "a large new text"
		});
		expected.text = testModel.get(4).name;
		compare(testObject.maxItemWidth, expected.implicitWidth, "maxItemWidth");
		testModel.append({
			"name": "Test"
		});
		compare(testObject.maxItemWidth, expected.implicitWidth, "maxItemWidth");
	}
	function test_model_without_autoConsider() {
		let testModel = testModelComponent.createObject(testCase);
		let testObject = testObjectComponent.createObject(testCase, {
			"autoConsiderItems": false,
			"considerFirstItem": true,
			"model": testModel
		});
		expected.text = testModel.get(0).name;
		compare(testObject.maxItemWidth, expected.implicitWidth, "maxItemWidth");
	}

	name: "test_GRepeater"
	visible: true
	when: windowShown

	Text {
		id: expected

	}
	Component {
		id: testModelComponent

		ListModel {
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
	}
	Component {
		id: testObjectComponent

		GRepeater {
			id: repeater

			property bool considerFirstItem: false

			Text {
				text: name

				Component.onCompleted: {
					if (repeater.considerFirstItem && index === 0) {
						repeater.addConsideredItem(this);
					}
				}
				Component.onDestruction: repeater.removeConsideredItem(this)
			}
		}
	}
}
