/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import QtQuick.Controls
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; ProxyCredentialsPopup {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_ProxyCredentialsPopup"
	visible: true
	when: windowShown

	Item {
		id: credentialContainer

		property bool confirmed
		property string password
		readonly property string proposedUser: "ProposedUser"
		property string user

		function confirmInput() {
			confirmed = true;
		}
	}
	ProxyCredentialsPopup {
		id: testObject

		property string lastEvent

		width: 1000

		onCancelled: lastEvent = "cancelled"
		onConfirmed: lastEvent = "confirmed"

		TestCase {
			function test_checkContainerValues() {
				testObject.open();
				testObject.accept();
				compare(testObject.lastEvent, "confirmed");
				testObject.open();
				testObject.cancel();
				compare(testObject.lastEvent, "cancelled");
				testObject.credentials = null;
				credentialContainer.user = "TestUser";
				credentialContainer.password = "TestPassword";
				credentialContainer.confirmed = false;
				testObject.credentials = credentialContainer;
				testObject.open();
				testObject.accept();
				compare(credentialContainer.user, "ProposedUser");
				compare(credentialContainer.password, "");
				verify(credentialContainer.confirmed);
				compare(testObject.lastEvent, "confirmed");
				testObject.credentials = null;
				credentialContainer.user = "TestUser";
				credentialContainer.password = "TestPassword";
				credentialContainer.confirmed = false;
				testObject.credentials = credentialContainer;
				testObject.open();
				testObject.cancel();
				compare(credentialContainer.user, "TestUser");
				compare(credentialContainer.password, "TestPassword");
				verify(credentialContainer.confirmed);
				compare(testObject.lastEvent, "cancelled");
			}

			when: windowShown
		}
	}
}
