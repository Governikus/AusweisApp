/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleLoadingView"
	id: parent

	function test_load_SectionPage() {
		var item = createTemporaryQmlObject("
			import Governikus.View 1.0;
			SectionPage {}
			", parent);
		item.destroy();
	}

	function test_load_FocusFrame() {
		var item = createTemporaryQmlObject("
			import Governikus.View 1.0;
			FocusFrame {}
			", parent);
		item.destroy();
	}

	function test_load_FocusPoint() {
		var item = createTemporaryQmlObject("
			import Governikus.View 1.0;
			FocusPoint {}
			", parent);
		item.destroy();
	}
}
