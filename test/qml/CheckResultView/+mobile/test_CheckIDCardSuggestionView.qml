/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Type.CheckIDCardModel 1.0

TestCase {
	id: testCase

	function createTestObject(result) {
		return createTemporaryQmlObject("import \"qrc:/qml/Governikus/CheckIDCardView/+mobile/\"; CheckIDCardSuggestionView { result: %1 }".arg(result), testCase);
	}
	function test_load(data) {
		let testObject = createTestObject(data.result);
		verify(testObject, "Object loaded");
	}
	function test_load_data() {
		return [{
				"result": CheckIDCardModel.NO_NFC
			}, {
				"result": CheckIDCardModel.UNKNOWN_CARD_DETECTED
			}, {
				"result": CheckIDCardModel.INSUFFICIENT_APDU_LENGTH
			}, {
				"result": CheckIDCardModel.CARD_ACCESS_FAILED
			}, {
				"result": CheckIDCardModel.PIN_DEACTIVATED
			}, {
				"result": CheckIDCardModel.PIN_SUSPENDED
			}, {
				"result": CheckIDCardModel.PIN_BLOCKED
			}, {
				"result": -1
			}];
	}

	name: "test_CheckIDCardSuggestionView"
	visible: true
	when: windowShown
}
