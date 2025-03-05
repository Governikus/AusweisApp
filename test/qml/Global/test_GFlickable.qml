/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GFlickable {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_positionViewAtItem() {
		let testFlickable = flickable.createObject(testCase);
		let childrenCount = testFlickable.columnChildren.length;
		for (let i = 0; i < childrenCount; ++i) {
			let child = testFlickable.columnChildren[i];
			testFlickable.positionViewAtItem(child);
			verify(testFlickable.contentY >= child.y);
			verify(testFlickable.contentY <= (child.y + child.height));
		}
		for (let j = childrenCount - 1; j >= 0; --j) {
			let child = testFlickable.columnChildren[j];
			testFlickable.positionViewAtItem(child);
			verify(testFlickable.contentY >= child.y);
			verify(testFlickable.contentY <= (child.y + child.height));
		}
	}

	name: "test_GFlickable"
	visible: true
	when: windowShown

	Component {
		id: flickable

		GFlickable {
			property alias columnChildren: column.children

			contentHeight: column.implicitHeight
			height: 50
			width: 50

			Column {
				id: column

				anchors.fill: parent
				spacing: 0

				Repeater {
					model: 10

					Rectangle {
						height: 50
						width: 50
					}
				}
			}
		}
	}
}
