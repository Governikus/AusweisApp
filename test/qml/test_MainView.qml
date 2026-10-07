/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
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
