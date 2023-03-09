/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtTest 1.15

TestCase {
	id: parent
	function test_load_ProgressView() {
		var item = createTemporaryQmlObject("
			import Governikus.ProgressView 1.0;
			ProgressView {}
			", parent);
		item.destroy();
	}

	name: "ModuleImportTest"
}
