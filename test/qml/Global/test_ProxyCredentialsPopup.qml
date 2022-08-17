/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtTest 1.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_ProxyCredentialsPopup"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; ProxyCredentialsPopup {}", testCase)
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}

	Item {
		id: credentialContainer

		readonly property string proposedUser: "ProposedUser"
		property string user
		property string password
		property bool confirmed

		function confirmInput() {
			confirmed = true
		}
	}

	ProxyCredentialsPopup {
		id: testObject

		width: 1000

		property string lastEvent
		onConfirmed: lastEvent = "confirmed"
		onCancelled: lastEvent = "cancelled"

		TestCase {
			when: windowShown

			function test_checkContainerValues() {
				testObject.open()
				testObject.accept()
				compare(testObject.lastEvent, "confirmed")

				testObject.open()
				testObject.cancel()
				compare(testObject.lastEvent, "cancelled")

				testObject.credentials = null
				credentialContainer.user = "TestUser"
				credentialContainer.password = "TestPassword"
				credentialContainer.confirmed = false
				testObject.credentials = credentialContainer
				testObject.open()
				testObject.accept()
				compare(credentialContainer.user, "ProposedUser")
				compare(credentialContainer.password, "")
				verify(credentialContainer.confirmed)
				compare(testObject.lastEvent, "confirmed")

				testObject.credentials = null
				credentialContainer.user = "TestUser"
				credentialContainer.password = "TestPassword"
				credentialContainer.confirmed = false
				testObject.credentials = credentialContainer
				testObject.open()
				testObject.cancel()
				compare(credentialContainer.user, "TestUser")
				compare(credentialContainer.password, "TestPassword")
				verify(credentialContainer.confirmed)
				compare(testObject.lastEvent, "cancelled")
			}
		}
	}
}
