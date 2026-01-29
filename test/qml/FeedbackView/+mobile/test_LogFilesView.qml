/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

import QtTest
import QtQuick

import Governikus.FeedbackView

Item {
	height: 400
	width: 400

	LogFilesView {
		id: logfilesView

		anchors.fill: parent
	}
	SignalSpy {
		id: signalSpy

		signalName: "logFilesListItemClicked"
		target: logfilesView
	}
	TestCase {
		id: testCase

		function test_load() {
			let testObject = createTemporaryQmlObject("import Governikus.FeedbackView; LogFilesView {}", testCase);
			verify(testObject, "Object loaded");
		}
		function test_whenItemClicked_signalIsEmitted() {
			testCase.Window.window.requestActivate();
			tryCompare(testCase.Window.window, "active", true);
			compare(signalSpy.count, 0);
			mouseClick(logfilesView, 20, 30);
			compare(signalSpy.count, 1);
		}

		name: "LogFilesViewTest"
		when: windowShown
	}
}
