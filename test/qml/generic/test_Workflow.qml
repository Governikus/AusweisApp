/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleLoadingWorkflow"
	id: parent

	function test_load_Workflow() {
		var item = createTemporaryQmlObject("
			import Governikus.Workflow 1.0;
			Workflow {}
			", parent);
		item.destroy();
	}

	function test_load_GeneralWorkflow() {
		var item = createTemporaryQmlObject("
			import Governikus.Workflow 1.0;
			GeneralWorkflow {}
			", parent);
		item.destroy();
	}
}
