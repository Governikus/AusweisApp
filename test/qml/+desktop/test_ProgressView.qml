/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtTest

TestCase {
	id: parent

	function test_load_ProgressView() {
		let item = createTemporaryQmlObject("
			import Governikus.ProgressView
			ProgressView {}
			", parent);
		item.destroy();
	}

	name: "ModuleImportTest"
}
