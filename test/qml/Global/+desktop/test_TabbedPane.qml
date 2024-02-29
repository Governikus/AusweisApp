/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import QtQml.Models
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; TabbedPane {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_noModel() {
		let testObject = createTestObject();
		compare(testObject.sectionsModel, undefined, "sectionsModel not set");
		compare(testObject.contentObjectModel, undefined, "contentObjectModel not set");
		compare(testObject.currentItemModel, null, "currentItemModel not set");
	}

	name: "test_TabbedPane"
	visible: true
	when: windowShown

	TabbedPane {
		id: testObject

		sectionsModel: ["Item 0", "Item 1", "Item 2"]

		contentObjectModel: ObjectModel {
			Component {
				Rectangle {
					color: "red"
				}
			}
			Component {
				Rectangle {
					color: "blue"
				}
			}
			Component {
				Rectangle {
					color: "black"
				}
			}
		}

		TestCase {
			function test_model() {
				compare(testObject.currentIndex, 0, "Initial index");
				compare(testObject.sectionCount, 3, "Section count");
			}
			function test_setIndex() {
				testObject.currentIndex = 2;
				compare(testObject.currentIndex, 2, "Set index 2");
				compare(testObject.currentItemModel.modelData, "Item 2", "Current item model: Item 2");
			}

			when: windowShown
		}
	}
}
