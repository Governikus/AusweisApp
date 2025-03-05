/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

import Governikus.TitleBar
import Governikus.View

TestCase {
	id: testCase

	function test_load() {
		let testObject = titleBar.createObject(testCase);
		verify(testObject, "Object loaded");
	}

	name: "test_TitleBar"
	visible: true
	when: windowShown

	Component {
		id: titleBar

		TitleBar {
			contentItem: SectionPage {
				title: "testCase"
			}
		}
	}
}
