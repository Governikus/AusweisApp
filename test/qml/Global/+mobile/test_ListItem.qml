/*
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_ListItem"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; ListItem {}", testCase)
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

	function test_text() {
		let testObject = createTestObject()

		compare(testObject.text, "", "Initial text: empty")
		testObject.text = "test"
		compare(testObject.text, "test", "text: test")

		compare(testObject.headerText, "", "Initial headerText: empty")
		testObject.headerText = "test"
		compare(testObject.headerText, "test", "headerText: test")

		compare(testObject.footerText, "", "Initial footerText: empty")
		testObject.footerText = "test"
		compare(testObject.footerText, "test", "footerText: test")
	}

	function test_icon() {
		let testObject = createTestObject()

		compare(testObject.icon, "", "Initial icon: empty")
		testObject.icon = "qrc:///images/material_check.svg"
		compare(testObject.icon, "qrc:///images/material_check.svg", "icon: qrc:///images/material_check.svg")

		verify(!testObject.tintIcon, "Initial tintIcon: false")
		testObject.tintIcon = true
		verify(testObject.tintIcon, "tintIcon: true")
	}

	function test_linkIcon() {
		let testObject = createTestObject()

		compare(testObject.showLinkIcon, Constants.is_layout_ios, "Initial showLinkIcon: false")
		testObject.showLinkIcon = false
		verify(!testObject.showLinkIcon, "tintIcon: false")
		testObject.showLinkIcon = true
		verify(testObject.showLinkIcon, "tintIcon: true")
	}

	function test_showSeparator() {
		let testObject = createTestObject()

		verify(testObject.showSeparator, "Initial showSeparator: true")
		testObject.showSeparator = false
		verify(!testObject.showSeparator, "showSeparator: false")
	}

	ListItem {
		id: testObject

		onClicked: text = "clicked"

		TestCase {
			when: windowShown

			function test_click() {
				testObject.text = ""
				compare(testObject.text, "", "Initial text: empty")
				verify(testObject.mouseAreaEnabled, "Initial mouseAreaEnabled: true")
				verify(!testObject.pressed, "Initial pressed: false")

				mousePress(testObject)
				verify(testObject.pressed, "pressed: true")
				mouseRelease(testObject)
				verify(!testObject.pressed, "pressed: false")
				compare(testObject.text, "clicked", "text: clicked")
			}

		}
	}

}
