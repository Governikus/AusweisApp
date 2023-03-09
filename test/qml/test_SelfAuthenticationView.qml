/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtTest 1.15

TestCase {
	id: parent
	function test_load_SelfAuthenticationView() {
		var item = createTemporaryQmlObject("
			import Governikus.SelfAuthenticationView 1.0;
			SelfAuthenticationView {}
			", parent);
		item.destroy();
	}

	name: "ModuleImportTest"
}
