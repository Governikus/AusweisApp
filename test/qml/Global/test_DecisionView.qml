/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
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
		testObject.primaryButton.text = "Agree";
		verify(testObject.primaryButton.visible, "Agree Button visible");
		verify(!testObject.secondaryButton.visible, "Disagree Button invisible");
	}
	function test_combination_buttons() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.primaryButton.text = "Agree";
		verify(testObject.primaryButton.visible, "Agree Button visible");
		verify(!testObject.secondaryButton.visible, "Disagree Button invisible");
		testObject.secondaryButton.text = "Disagree";
		verify(testObject.primaryButton.visible, "Agree Button visible");
		verify(testObject.secondaryButton.visible, "Disagree Button visible");
	}
	function test_default_buttons() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		verify(!testObject.primaryButton.visible, "Agree Button invisible");
		verify(!testObject.secondaryButton.visible, "Disagree Button invisible");
	}
	function test_disagree_button() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.secondaryButton.text = "Disagree";
		verify(!testObject.primaryButton.visible, "Agree Button invisible");
		verify(testObject.secondaryButton.visible, "Disagree Button visible");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_DecisionView"
	visible: true
	when: windowShown
}
