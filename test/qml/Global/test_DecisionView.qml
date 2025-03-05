/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; DecisionView { title: \"testCase\" }", testCase);
	}
	function test_agree_button() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.agreeButtonText = "Agree";
		verify(testObject.agreeButton.visible, "Agree Button visible");
		verify(!testObject.disagreeButton.visible, "Disagree Button invisible");
	}
	function test_combination_buttons() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.agreeButtonText = "Agree";
		verify(testObject.agreeButton.visible, "Agree Button visible");
		verify(!testObject.disagreeButton.visible, "Disagree Button invisible");
		testObject.disagreeButtonText = "Disagree";
		verify(testObject.agreeButton.visible, "Agree Button visible");
		verify(testObject.disagreeButton.visible, "Disagree Button visible");
	}
	function test_default_buttons() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		verify(!testObject.agreeButton.visible, "Agree Button invisible");
		verify(!testObject.disagreeButton.visible, "Disagree Button invisible");
	}
	function test_disagree_button() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.disagreeButtonText = "Disagree";
		verify(!testObject.agreeButton.visible, "Agree Button invisible");
		verify(testObject.disagreeButton.visible, "Disagree Button visible");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_DecisionView"
	visible: true
	when: windowShown
}
