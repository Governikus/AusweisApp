/*
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_StoreFeedbackPopup() {
		var item = createTemporaryQmlObject("
			import Governikus.FeedbackView 1.0;
			StoreFeedbackPopup {}
			", parent);
		item.destroy();
	}
}
