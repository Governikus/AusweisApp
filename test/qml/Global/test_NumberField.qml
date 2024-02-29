/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; NumberField {}", testCase);
	}
	function test_append() {
		let testObject = createTestObject();
		testObject.append(1);
		compare(testObject.number, "1", "Append: 1");
		testObject.append(2);
		compare(testObject.number, "12", "Append: 2");
		testObject.append(3);
		testObject.append(4);
		testObject.append(5);
		testObject.append(6);
		compare(testObject.number, "123456", "Number: 123456");
		testObject.append(7);
		compare(testObject.number, "123456", "Number: 123456");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_number() {
		let testObject = createTestObject();
		compare(testObject.number, "", "Initial number empty");
		testObject.number = "123456";
		compare(testObject.number, "123456", "Set number: 123456");
	}
	function test_passwordLength() {
		let testObject = createTestObject();
		compare(testObject.passwordLength, 6);
		compare(testObject.number, "");
		testObject.number = "123456";
		compare(testObject.number, "123456");
		testObject.passwordLength = 5;
		compare(testObject.passwordLength, 5);
		compare(testObject.number, "");
	}
	function test_removeLast() {
		let testObject = createTestObject();
		compare(testObject.number, "", "Empty number");
		testObject.removeLast();
		compare(testObject.number, "", "Empty number");
		testObject.number = "123456";
		compare(testObject.number, "123456", "Number: 123456");
		testObject.removeLast();
		compare(testObject.number, "12345", "Number: 12345");
		testObject.removeLast();
		testObject.removeLast();
		testObject.removeLast();
		testObject.removeLast();
		testObject.removeLast();
		compare(testObject.number, "", "Empty number");
	}

	name: "test_NumberField"
	visible: true
	when: windowShown
}
