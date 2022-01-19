/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_ProgressView() {
		var item = createTemporaryQmlObject("
			import Governikus.ProgressView 1.0;
			ProgressView {}
			", parent);
		item.destroy();
	}
}
