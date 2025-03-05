/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global
import Governikus.Type

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; LocationButton {
			a11yDescription: \"a11yDescription\"
			a11yName: \"a11yName\"
			image: \"qrc:///images/npa.svg\"
			language: \"language\"
			languageText: \"languageText\"
		}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_LocationButton"
	visible: true
	when: windowShown
}
