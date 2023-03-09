/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtTest 1.15

TestCase {
	id: parent
	function test_load_MainView() {
		var item = createTemporaryQmlObject("
			import Governikus.MainView 1.0;
			MainView {}
			", parent);
		item.destroy();
	}

	name: "ModuleImportTest"
}
