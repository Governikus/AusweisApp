/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
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
