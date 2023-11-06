/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtTest

TestCase {
	id: parent

	function test_load_MoreView() {
		let item = createTemporaryQmlObject("
			import Governikus.MoreView
			MoreView {}
			", parent);
		item.destroy();
	}

	name: "ModuleImportTest"
}
