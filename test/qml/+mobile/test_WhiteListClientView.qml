/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_WhiteListSurveyView() {
		var item = createTemporaryQmlObject("
			import Governikus.WhiteListClient 1.0;
			WhiteListSurveyView {}
			", parent);
		item.destroy();
	}
}
