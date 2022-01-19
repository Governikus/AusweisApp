/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12

import Governikus.Type.UiModule 1.0

TestCase {
	name: "UiModule enumeration"
	id: parent

	function test_load_UiModule() {
		var item = createTemporaryQmlObject("
			import QtQuick 2.12
			import Governikus.Type.UiModule 1.0
			Item {
				readonly property int v1: UiModule.CURRENT
				readonly property int v2: UiModule.DEFAULT
				readonly property int v3: UiModule.IDENTIFY
				readonly property int v4: UiModule.PINMANAGEMENT
				readonly property int v5: UiModule.SETTINGS
			}
			", parent);
		item.destroy();
	}

	function test_enumeration() {
		verify(UiModule.CURRENT !== UiModule.DEFAULT)
		verify(UiModule.DEFAULT !== UiModule.IDENTIFY)
		verify(UiModule.IDENTIFY !== UiModule.PINMANAGEMENT)
		verify(UiModule.PINMANAGEMENT !== UiModule.SETTINGS)
	}
}
