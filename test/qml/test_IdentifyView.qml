/*
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12
import QtQuick 2.12

TestCase {
	id: appWindow

	name: "test_IdentifyView"
	visible: true
	when: windowShown

	property var menuBar: Item {
		function updateActions() {
		}
	}

	function test_load() {
		createTemporaryQmlObject("
			import Governikus.IdentifyView 1.0;
			IdentifyView {}
			", appWindow)
	}
}
