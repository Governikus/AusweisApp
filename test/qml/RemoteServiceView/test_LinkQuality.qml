/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.RemoteServiceView
import Governikus.Type

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.RemoteServiceView; LinkQuality {}", testCase);
	}
	function test_image(data) {
		SettingsModel.useAnimations = data.useAnimations;
		let testObject = createTestObject();
		testObject.inactive = !!data.inactive;
		testObject.percent = data.percent;
		verify(testObject.source.toString().includes(data.imageContains));
	}
	function test_image_data() {
		return [
			{
				"tag": "With Animations, Inactive 0 %",
				"useAnimations": true,
				"inactive": true,
				"percent": 0,
				"imageContains": "inactive"
			},
			{
				"tag": "With Animations, Inactive 100 %",
				"useAnimations": true,
				"inactive": true,
				"percent": 100,
				"imageContains": "inactive"
			},
			{
				"tag": "With Animations, 10 %",
				"useAnimations": true,
				"percent": 10,
				"imageContains": "inactive"
			},
			{
				"tag": "With Animations, 30 %",
				"useAnimations": true,
				"percent": 30,
				"imageContains": "_25_"
			},
			{
				"tag": "With Animations, 45 %",
				"useAnimations": true,
				"percent": 45,
				"imageContains": "_50_"
			},
			{
				"tag": "With Animations, 70 %",
				"useAnimations": true,
				"percent": 70,
				"imageContains": "_75_"
			},
			{
				"tag": "With Animations, 90 %",
				"useAnimations": true,
				"percent": 90,
				"imageContains": "_100_"
			},
			{
				"tag": "Without Animations, Inactive 0 %",
				"useAnimations": false,
				"inactive": true,
				"percent": 0,
				"imageContains": "inactive"
			},
			{
				"tag": "Without Animations, Inactive 100 %",
				"useAnimations": false,
				"inactive": true,
				"percent": 100,
				"imageContains": "inactive"
			},
			{
				"tag": "Without Animations, 10 %",
				"useAnimations": false,
				"percent": 10,
				"imageContains": "inactive"
			},
			{
				"tag": "Without Animations, 30 %",
				"useAnimations": false,
				"percent": 30,
				"imageContains": "_100_"
			},
			{
				"tag": "Without Animations, 45 %",
				"useAnimations": false,
				"percent": 45,
				"imageContains": "_100_"
			},
			{
				"tag": "Without Animations, 70 %",
				"useAnimations": false,
				"percent": 70,
				"imageContains": "_100_"
			},
			{
				"tag": "Without Animations, 90 %",
				"useAnimations": false,
				"percent": 90,
				"imageContains": "_100_"
			}
		];
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_LinkQuality"
	visible: true
	when: windowShown
}
