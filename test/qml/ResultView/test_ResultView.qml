/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtTest
import Governikus.Global
import Governikus.ResultView
import Governikus.Type

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.ResultView; ResultView { title: \"ResultView\" }", testCase);
	}
	function test_PRSHintsVisibility() {
		let i = GlobalStatusCode.Unknown_Error;
		for (i; i <= GlobalStatusCode.IfdConnector_RemoteHostRefusedConnection; i++) {
			const result = createTemporaryQmlObject("
				import Governikus.ResultView
				ResultView {
					hintBoxesTitle: \"PRSHints\"
					title: \"ResultView\"
					statusCode: %1
				}".arg(i), testCase);
			const prsHint = findChild(result, "PRSHint");
			verify(prsHint !== null);
			let hints = prsHint.parent;
			verify(hints !== null);
			if (i === GlobalStatusCode.Card_ValidityVerificationFailed)
				verify(!hints.visible);
			else
				verify(hints.visible);
		}
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_ResultView"
	visible: true
	when: windowShown
}
