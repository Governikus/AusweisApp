/*
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_NumberField"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; NumberField {}", testCase)
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

	function test_number() {
		let testObject = createTestObject()

		compare(testObject.number, "", "Initial number empty")
		testObject.number = "123456"
		compare(testObject.number, "123456", "Set number: 123456")
	}

	function test_passwordLength() {
		let testObject = createTestObject()

		compare(testObject.passwordLength, 6)
		compare(testObject.number, "")

		testObject.number = "123456"
		compare(testObject.number, "123456")

		testObject.passwordLength = 5
		compare(testObject.passwordLength, 5)
		compare(testObject.number, "")
	}

	function test_append() {
		let testObject = createTestObject()

		testObject.append(1)
		compare(testObject.number, "1", "Append: 1")

		testObject.append(2)
		compare(testObject.number, "12", "Append: 2")

		testObject.append(3)
		testObject.append(4)
		testObject.append(5)
		testObject.append(6)
		compare(testObject.number, "123456", "Number: 123456")

		testObject.append(7)
		compare(testObject.number, "123456", "Number: 123456")
	}

	function test_removeLast() {
		let testObject = createTestObject()

		compare(testObject.number, "", "Empty number")
		testObject.removeLast()
		compare(testObject.number, "", "Empty number")

		testObject.number = "123456"
		compare(testObject.number, "123456", "Number: 123456")
		testObject.removeLast()
		compare(testObject.number, "12345", "Number: 12345")

		testObject.removeLast()
		testObject.removeLast()
		testObject.removeLast()
		testObject.removeLast()
		testObject.removeLast()
		compare(testObject.number, "", "Empty number")
	}

	function test_validInput() {
		let testObject = createTestObject()

		verify(!testObject.validInput, "Initial validInput: false")
		verify(testObject.confirmedInput, "Initial confirmedInput: true")

		testObject.number = "123456"
		testObject.inputConfirmation = "123456"
		verify(testObject.validInput, "validInput: true")
		verify(testObject.confirmedInput, "confirmedInput: true")

		testObject.number = "12345"
		testObject.inputConfirmation = "123456"
		verify(!testObject.validInput, "short password validInput: false")
		verify(testObject.confirmedInput, "short password confirmedInput: false")

		testObject.number = "1"
		testObject.inputConfirmation = "123456"
		verify(!testObject.validInput, "mismatch password validInput: false")
		verify(testObject.confirmedInput, "mismatch password confirmedInput: false")

		testObject.number = "12345a"
		testObject.inputConfirmation = "123456"
		verify(!testObject.validInput, "invalid character password validInput: false")
		verify(!testObject.confirmedInput, "invalid character confirmedInput: false")
	}

}
