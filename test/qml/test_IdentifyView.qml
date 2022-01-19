/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12
import QtQuick 2.12
import QtQuick.Controls 2.12;

TestCase {
	name: "test_IdentifyView"
	when: windowShown

	ApplicationWindow {
		id: appWindow

		menuBar: Item {
			function updateActions() {
			}
		}
	}

	function test_load() {
		createTemporaryQmlObject("
			import Governikus.IdentifyView 1.0;
			IdentifyView {}
			", appWindow)
	}
}
