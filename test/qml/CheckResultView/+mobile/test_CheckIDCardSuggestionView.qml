/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Type

TestCase {
	id: testCase

	function test_load(data) {
		const component = Qt.createComponent(importPrefix + "/Governikus/CheckIDCardView/CheckIDCardSuggestionView.qml");
		verify(component.status === Component.Ready, "Component loaded");
		const testObject = component.createObject(parent, {
			result: data.result
		});
		verify(testObject, "Object loaded");
	}
	function test_load_data() {
		return [
			{
				"result": CheckIDCardModel.Result.NO_READER
			},
			{
				"result": CheckIDCardModel.Result.UNKNOWN_CARD_DETECTED
			},
			{
				"result": CheckIDCardModel.Result.INSUFFICIENT_APDU_LENGTH
			},
			{
				"result": CheckIDCardModel.Result.CARD_ACCESS_FAILED
			},
			{
				"result": CheckIDCardModel.Result.PIN_DEACTIVATED
			},
			{
				"result": CheckIDCardModel.Result.PIN_SUSPENDED
			},
			{
				"result": CheckIDCardModel.Result.PIN_BLOCKED
			},
			{
				"result": -1
			}
		];
	}

	name: "test_CheckIDCardSuggestionView"
	visible: true
	when: windowShown
}
