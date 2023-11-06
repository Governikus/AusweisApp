/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; DecisionView {}", testCase);
	}
	function test_agree_button() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.style = DecisionView.ButtonStyle.AgreeButton;
		verify(testObject.agreeButton.visible, "Agree Button visible");
		verify(!testObject.disagreeButton.visible, "Disagree Button invisible");
	}
	function test_combination_buttons() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.style = DecisionView.ButtonStyle.AgreeButton;
		verify(testObject.agreeButton.visible, "Agree Button visible");
		verify(!testObject.disagreeButton.visible, "Disagree Button invisible");
		testObject.style = DecisionView.ButtonStyle.DisagreeButton;
		verify(!testObject.agreeButton.visible, "Agree Button invisible");
		verify(testObject.disagreeButton.visible, "Disagree Button visible");
	}
	function test_default_buttons() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		verify(testObject.agreeButton.visible, "Agree Button visible");
		verify(testObject.disagreeButton.visible, "Disagree Button visible");
	}
	function test_disagree_button() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.style = DecisionView.ButtonStyle.DisagreeButton;
		verify(!testObject.agreeButton.visible, "Agree Button invisible");
		verify(testObject.disagreeButton.visible, "Disagree Button visible");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_no_buttons() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.style = DecisionView.ButtonStyle.NoButtons;
		verify(!testObject.agreeButton.visible, "Agree Button invisible");
		verify(!testObject.disagreeButton.visible, "Disagree Button invisible");
	}

	name: "test_DecisionView"
	visible: true
	when: windowShown
}
