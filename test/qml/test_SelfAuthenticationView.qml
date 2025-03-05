/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */
import QtTest

TestCase {
	id: parent

	function test_load_SelfAuthenticationView() {
		let item = createTemporaryQmlObject("
			import Governikus.SelfAuthenticationView
			SelfAuthenticationView {}
			", parent);
		item.destroy();
	}

	name: "ModuleImportTest"
}
