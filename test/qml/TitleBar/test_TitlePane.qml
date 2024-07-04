/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	function test_load() {
		const component = Qt.createComponent(importPrefix + "/Governikus/TitleBar/TitlePane.qml");
		verify(component.status === Component.Ready, "Component loaded");
		const testObject = component.createObject(parent, {});
		verify(testObject, "Object loaded");
	}
}
