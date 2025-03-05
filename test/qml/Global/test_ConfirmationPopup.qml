/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import QtQuick.Controls
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; ConfirmationPopup {}", testCase);
	}
	function test_cancelButtonText() {
		let testObject = createTestObject();
		compare(testObject.cancelButtonText, "Cancel", "Initial cancelButtonText: Cancel");
		testObject.cancelButtonText = "test";
		compare(testObject.cancelButtonText, "test", "cancelButtonText: test");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_okButtonText() {
		let testObject = createTestObject();
		compare(testObject.okButtonText, "OK", "Initial okButtonText: OK");
		testObject.okButtonText = "test";
		compare(testObject.okButtonText, "test", "okButtonText: test");
	}
	function test_show() {
		let testObject = createTestObject();
		testObject.open();
		tryVerify(function () {
			return testObject.opened;
		});
		testObject.close();
		tryVerify(function () {
			return !testObject.opened;
		});
	}
	function test_style() {
		let testObject = createTestObject();
		compare(testObject.style, ConfirmationPopup.PopupStyle.OkButton | ConfirmationPopup.PopupStyle.CancelButton, "Initial style: OkButton | CancelButton");
		testObject.style = ConfirmationPopup.PopupStyle.OkButton;
		compare(testObject.style, ConfirmationPopup.PopupStyle.OkButton, "style: OkButton");
		testObject.style = ConfirmationPopup.PopupStyle.CancelButton;
		compare(testObject.style, ConfirmationPopup.PopupStyle.CancelButton, "style: CancelButton");
		testObject.style = ConfirmationPopup.PopupStyle.NoButtons;
		compare(testObject.style, ConfirmationPopup.PopupStyle.NoButtons, "style: NoButtons");
	}
	function test_text() {
		let testObject = createTestObject();
		compare(testObject.text, "", "Initial text: empty");
		testObject.text = "test";
		compare(testObject.text, "test", "text: test");
	}
	function test_title() {
		let testObject = createTestObject();
		compare(testObject.title, "", "Initial title: empty");
		testObject.title = "test";
		compare(testObject.title, "test", "title: test");
	}

	name: "test_ConfirmationPopup"
	visible: true
	when: windowShown

	ConfirmationPopup {
		id: testObject

		width: 1000

		onCancelled: text = "cancelled"
		onConfirmed: text = "confirmed"

		TestCase {
			function test_accept() {
				verify(testObject.closePolicy !== Popup.NoAutoClose, "autoClose enabled");
				testObject.text = "";
				compare(testObject.text, "", "text: empty");
				testObject.open();
				tryVerify(function () {
					return testObject.opened;
				});
				testObject.accept();
				tryVerify(function () {
					return !testObject.opened;
				});
				compare(testObject.text, "confirmed", "text: confirmed");
			}
			function test_cancel() {
				verify(testObject.closePolicy !== Popup.NoAutoClose, "autoClose enabled");
				testObject.text = "";
				compare(testObject.text, "", "text: empty");
				testObject.open();
				tryVerify(function () {
					return testObject.opened;
				});
				testObject.cancel();
				tryVerify(function () {
					return !testObject.opened;
				});
				compare(testObject.text, "cancelled", "text: cancelled");
			}

			when: windowShown
		}
	}
}
