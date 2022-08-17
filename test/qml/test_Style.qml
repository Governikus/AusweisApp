/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.15

TestCase {
	name: "ModuleLoadingStyle"
	id: parent

	function test_load_Style() {
		var item = createTemporaryQmlObject("
			import QtQuick 2.15;
			import Governikus.Style 1.0;
			Item {}
			", parent);
		item.destroy();
	}
}
