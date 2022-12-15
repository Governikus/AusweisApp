/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Global 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; DecisionView {}", testCase);
	}
	function test_agree_button() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.style = DecisionView.ButtonStyle.AgreeButton;
		verify(testObject.agreeButton.visible, "Agree Button visible");
		verify(!testObject.disagreeButton.visible, "Disagree Button invisible");
		verify(!testObject.neutralButton.visible, "Neutral Button invisible");
	}
	function test_all_buttons() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.style = DecisionView.ButtonStyle.AllButtons;
		verify(testObject.agreeButton.visible, "Agree Button visible");
		verify(testObject.disagreeButton.visible, "Disagree Button visible");
		verify(testObject.neutralButton.visible, "Neutral Button visible");
	}
	function test_combination_buttons() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.style = DecisionView.ButtonStyle.AgreeButton | DecisionView.ButtonStyle.NeutralButton;
		verify(testObject.agreeButton.visible, "Agree Button visible");
		verify(!testObject.disagreeButton.visible, "Disagree Button invisible");
		verify(testObject.neutralButton.visible, "Neutral Button visible");
		testObject.style = DecisionView.ButtonStyle.NeutralButton | DecisionView.ButtonStyle.DisagreeButton;
		verify(!testObject.agreeButton.visible, "Agree Button invisible");
		verify(testObject.disagreeButton.visible, "Disagree Button visible");
		verify(testObject.neutralButton.visible, "Neutral Button visible");
	}
	function test_default_buttons() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		verify(testObject.agreeButton.visible, "Agree Button visible");
		verify(testObject.disagreeButton.visible, "Disagree Button visible");
		verify(!testObject.neutralButton.visible, "Neutral Button invisible");
	}
	function test_disagree_button() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.style = DecisionView.ButtonStyle.DisagreeButton;
		verify(!testObject.agreeButton.visible, "Agree Button invisible");
		verify(testObject.disagreeButton.visible, "Disagree Button visible");
		verify(!testObject.neutralButton.visible, "Neutral Button invisible");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_neutral_button() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.style = DecisionView.ButtonStyle.NeutralButton;
		verify(!testObject.agreeButton.visible, "Agree Button invisible");
		verify(!testObject.disagreeButton.visible, "Disagree Button invisible");
		verify(testObject.neutralButton.visible, "Neutral Button visible");
	}
	function test_no_buttons() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		testObject.style = DecisionView.ButtonStyle.NoButtons;
		verify(!testObject.agreeButton.visible, "Agree Button invisible");
		verify(!testObject.disagreeButton.visible, "Disagree Button invisible");
		verify(!testObject.neutralButton.visible, "Neutral Button invisible");
	}

	name: "test_DecisionView"
	visible: true
	when: windowShown
}
