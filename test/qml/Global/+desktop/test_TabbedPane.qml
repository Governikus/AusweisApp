/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12
import QtQml.Models 2.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_TabbedPane"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; TabbedPane {}", testCase)
	}

	TestCase {
		when: testCase.completed
		optional: true

		function benchmark_load() {
			let testObject = createTestObject()
			testObject.destroy()
		}
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}

	function test_noModel() {
		let testObject = createTestObject()

		compare(testObject.sectionsModel, undefined, "sectionsModel not set")
		compare(testObject.contentObjectModel, undefined, "contentObjectModel not set")
		compare(testObject.currentItemModel, null, "currentItemModel not set")
	}

	TabbedPane {
		id: testObject

		sectionsModel: ["Item 0", "Item 1", "Item 2"]
		contentObjectModel: ObjectModel {
			Component { Rectangle { color: "red" } }
			Component { Rectangle { color: "blue" } }
			Component { Rectangle { color: "black" } }
		}

		TestCase {
			when: windowShown

			function test_model() {
				compare(testObject.currentIndex, 0, "Initial index")
				compare(testObject.sectionCount, 3, "Section count")
			}

			function test_setIndex() {
				testObject.currentIndex = 2

				compare(testObject.currentIndex, 2, "Set index 2")
				compare(testObject.currentItemModel.modelData, "Item 2", "Current item model: Item 2")
			}
		}
	}
}
