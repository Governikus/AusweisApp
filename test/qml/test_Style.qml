/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

import QtTest

TestCase {
	id: parent

	function test_load_Style() {
		let item = createTemporaryQmlObject("
			import QtQuick
			import Governikus.Style
			Item {}
			", parent);
		item.destroy();
	}

	name: "ModuleLoadingStyle"
}
