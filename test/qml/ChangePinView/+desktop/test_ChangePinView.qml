/*
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.ChangePinView 1.0

TestCase {
	id: testCase

	name: "test_ChangePinView"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick 2.12
			import Governikus.ChangePinView 1.0;

			Item {
				id: appWindow

				property alias _d: view._d
				property var menuBar: Item {
					function updateActions() {}
				}

				ChangePinView {
					id: view
				}
			}
			", testCase);
	}

	TestCase {
		when: testCase.completed
		optional: true

		function benchmark_load() {
			let testObject = createTestObject()
			testObject.destroy()
		}
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}

	function test_subview_Workflow() {
		let testObject = createTestObject()
		testObject._d.view = ChangePinView.SubViews.Workflow
		compare(testObject._d.view, ChangePinView.SubViews.Workflow, "Subview shown")
	}

	function test_subview_Password() {
		let testObject = createTestObject()
		testObject._d.view = ChangePinView.SubViews.Password
		compare(testObject._d.view, ChangePinView.SubViews.Password, "Subview shown")
	}

	function test_subview_PasswordInfo() {
		let testObject = createTestObject()
		testObject._d.view = ChangePinView.SubViews.PasswordInfo
		compare(testObject._d.view, ChangePinView.SubViews.PasswordInfo, "Subview shown")
	}

	function test_subview_Progress() {
		let testObject = createTestObject()
		testObject._d.view = ChangePinView.SubViews.Progress
		compare(testObject._d.view, ChangePinView.SubViews.Progress, "Subview shown")
	}

	function test_subview_CardPosition() {
		let testObject = createTestObject()
		testObject._d.view = ChangePinView.SubViews.CardPosition
		compare(testObject._d.view, ChangePinView.SubViews.CardPosition, "Subview shown")
	}

	function test_subview_InputError() {
		let testObject = createTestObject()
		testObject._d.view = ChangePinView.SubViews.InputError
		compare(testObject._d.view, ChangePinView.SubViews.InputError, "Subview shown")
	}

	function test_subview_Data() {
		let testObject = createTestObject()
		testObject._d.view = ChangePinView.SubViews.Data
		compare(testObject._d.view, ChangePinView.SubViews.Data, "Subview shown")
	}

	function test_subview_PinUnlocked() {
		let testObject = createTestObject()
		testObject._d.view = ChangePinView.SubViews.PinUnlocked
		compare(testObject._d.view, ChangePinView.SubViews.PinUnlocked, "Subview shown")
	}

	function test_subview_Result() {
		let testObject = createTestObject()
		testObject._d.view = ChangePinView.SubViews.Result
		compare(testObject._d.view, ChangePinView.SubViews.Result, "Subview shown")
	}

	function test_subview_ReturnToMain() {
		let testObject = createTestObject()
		testObject._d.view = ChangePinView.SubViews.ReturnToMain
		compare(testObject._d.view, ChangePinView.SubViews.ReturnToMain, "Subview shown")
	}

	function test_subview_ReaderSettings() {
		let testObject = createTestObject()
		testObject._d.view = ChangePinView.SubViews.ReaderSettings
		compare(testObject._d.view, ChangePinView.SubViews.ReaderSettings, "Subview shown")
	}

}
