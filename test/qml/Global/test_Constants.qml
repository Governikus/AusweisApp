/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_Constants"
	visible: true
	when: windowShown

	function test_constantsExist() {
		compare(Constants.white, "#ffffff", "Constants.white exists")
		compare(Constants.black, "#000000", "Constants.black exists")
	}

}
