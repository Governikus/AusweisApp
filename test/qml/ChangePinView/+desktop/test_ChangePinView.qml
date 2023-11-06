/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.ChangePinView

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick
			import QtQuick.Controls
			import Governikus.ChangePinView

			ApplicationWindow {
				readonly property alias _d: view._d

				menuBar: Item {
					function updateActions() {}
				}

				ChangePinView {
					id: view
				}
			}
		", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_subview_CardPosition() {
		let testObject = createTestObject();
		testObject._d.view = ChangePinView.SubViews.CardPosition;
		compare(testObject._d.view, ChangePinView.SubViews.CardPosition, "Subview shown");
	}
	function test_subview_Data() {
		let testObject = createTestObject();
		testObject._d.view = ChangePinView.SubViews.Data;
		compare(testObject._d.view, ChangePinView.SubViews.Data, "Subview shown");
	}
	function test_subview_InputError() {
		let testObject = createTestObject();
		testObject._d.view = ChangePinView.SubViews.InputError;
		compare(testObject._d.view, ChangePinView.SubViews.InputError, "Subview shown");
	}
	function test_subview_Password() {
		let testObject = createTestObject();
		testObject._d.view = ChangePinView.SubViews.Password;
		compare(testObject._d.view, ChangePinView.SubViews.Password, "Subview shown");
	}
	function test_subview_PasswordInfo() {
		let testObject = createTestObject();
		testObject._d.view = ChangePinView.SubViews.PasswordInfo;
		compare(testObject._d.view, ChangePinView.SubViews.PasswordInfo, "Subview shown");
	}
	function test_subview_PinUnlocked() {
		let testObject = createTestObject();
		testObject._d.view = ChangePinView.SubViews.PinUnlocked;
		compare(testObject._d.view, ChangePinView.SubViews.PinUnlocked, "Subview shown");
	}
	function test_subview_Progress() {
		let testObject = createTestObject();
		testObject._d.view = ChangePinView.SubViews.Progress;
		compare(testObject._d.view, ChangePinView.SubViews.Progress, "Subview shown");
	}
	function test_subview_ReaderSettings() {
		let testObject = createTestObject();
		testObject._d.view = ChangePinView.SubViews.ReaderSettings;
		compare(testObject._d.view, ChangePinView.SubViews.ReaderSettings, "Subview shown");
	}
	function test_subview_Result() {
		let testObject = createTestObject();
		testObject._d.view = ChangePinView.SubViews.Result;
		compare(testObject._d.view, ChangePinView.SubViews.Result, "Subview shown");
	}
	function test_subview_ReturnToMain() {
		let testObject = createTestObject();
		testObject._d.view = ChangePinView.SubViews.ReturnToMain;
		compare(testObject._d.view, ChangePinView.SubViews.ReturnToMain, "Subview shown");
	}
	function test_subview_Workflow() {
		let testObject = createTestObject();
		testObject._d.view = ChangePinView.SubViews.Workflow;
		compare(testObject._d.view, ChangePinView.SubViews.Workflow, "Subview shown");
	}

	name: "test_ChangePinView"
	visible: true
	when: windowShown
}
