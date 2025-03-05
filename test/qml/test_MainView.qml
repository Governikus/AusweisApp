/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */
import QtTest

TestCase {
	id: parent

	function test_load_MainView() {
		let item = createTemporaryQmlObject("
			import Governikus.MainView
			MainView {}
			", parent);
		item.destroy();
	}

	name: "ModuleImportTest"
}
