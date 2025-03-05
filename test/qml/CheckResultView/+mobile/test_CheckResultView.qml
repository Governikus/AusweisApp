/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.CheckResultView; import Governikus.Type; CheckResultView { pluginType: ReaderManagerPluginType.PCSC; title: \"CheckResultView\"; result: 0 }", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_CheckResultView"
	visible: true
	when: windowShown
}
