/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function test_constantsExist() {
		compare(Constants.white, "#ffffff", "Constants.white exists");
		compare(Constants.black, "#000000", "Constants.black exists");
	}

	name: "test_Constants"
	visible: true
	when: windowShown
}
