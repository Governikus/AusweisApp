/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQml 2.15
import QtQuick 2.15
import QtTest 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; GButton {}", testCase);
	}
	function test_enableButton() {
		let testObject = createTestObject();
		verify(testObject.enableButton, "enableButton: true");
		verify(testObject.enabled, "enabled: true");
		testObject.enableButton = false;
		verify(!testObject.enableButton, "enableButton: false");
		verify(testObject.enabled, "enabled: true");
	}
	function test_icon() {
		let testObject = createTestObject();
		compare(testObject.icon.source, "", "Initial no image");
		testObject.icon.source = "qrc:///images/material_check.svg";
		compare(testObject.icon.source, "qrc:///images/material_check.svg", "Image: qrc:///images/material_check.svg");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_size(data) {
		let button = createTemporaryQmlObject("import Governikus.Global 1.0; GButton {icon.source: \"" + data.icon + "\"; text: \"" + data.text + "\"}", testCase);
		tryCompare(button, "height", data.height);
		tryCompare(button, "width", data.width);
	}
	function test_size_data() {
		let text = createTemporaryQmlObject("import Governikus.Global 1.0; import Governikus.Style 1.0; GText {textStyle: Style.text.button; text: \"test test test test test test\"}", testCase);
		verify(waitForRendering(text));
		let longTextWidth = Math.ceil(text.width);
		return [{
				"tag": "noIconNoText",
				"icon": "",
				"text": "",
				"height": 12,
				"width": 16
			}, {
				"tag": "noIconSmallText",
				"icon": "",
				"text": "t",
				"height": Constants.is_desktop ? 39 : 40,
				"width": Constants.is_desktop ? 59 : 112
			}, {
				"tag": "noIconLongText",
				"icon": "",
				"text": "test test test test test test",
				"height": Constants.is_desktop ? 39 : 40,
				"width": 16 + longTextWidth
			}, {
				"tag": "withIconNoText",
				"icon": "qrc:///images/identify.svg",
				"text": "",
				"height": Constants.is_desktop ? 39 : 40,
				"width": Constants.is_desktop ? 43 : 44
			}, {
				"tag": "withIconSmallText",
				"icon": "qrc:///images/identify.svg",
				"text": "t",
				"height": Constants.is_desktop ? 39 : 40,
				"width": Constants.is_desktop ? (Qt.platform.os === "linux" ? 64 : 59) : 112
			}, {
				"tag": "withIconLongText",
				"icon": "qrc:///images/identify.svg",
				"text": "test test test test test test",
				"height": Constants.is_desktop ? 39 : 40,
				"width": (Constants.is_desktop ? 49 : 54) + longTextWidth
			}];
	}
	function test_text() {
		let testObject = createTestObject();
		compare(testObject.text, "", "Initial empty text");
		testObject.text = "test";
		compare(testObject.text, "test", "text: test");
	}
	function test_textStyle() {
		let testObject = createTestObject();
		compare(testObject.textStyle, Style.text.button, "Initial textStyle: button");
		testObject.textStyle = Style.text.hint_warning;
		compare(testObject.textStyle, Style.text.hint_warning, "textStyle: hint_warning");
	}
	function test_tooltipText() {
		let testObject = createTestObject();
		compare(testObject.enabledTooltipText, "", "Initial enabledTooltipText: empty");
		compare(testObject.disabledTooltipText, "", "Initial disabledTooltipText: empty");
		testObject.enabledTooltipText = "enabledTooltipText";
		testObject.disabledTooltipText = "disabledTooltipText";
		compare(testObject.enabledTooltipText, "enabledTooltipText", "Initial enabledTooltipText: empty");
		compare(testObject.disabledTooltipText, "disabledTooltipText", "Initial disabledTooltipText: empty");
		testObject.enableButton = false;
	}

	name: "test_GButton"
	visible: true
	when: windowShown

	GButton {
		id: testObject
		icon.source: "qrc:///images/material_check.svg"
		text: "test"

		TestCase {
			function test_click() {
				verify(!testObject.pressed, "pressed: false");
				mousePress(testObject);
				tryVerify(function () {
						return testObject.pressed;
					});
				mouseRelease(testObject);
				tryVerify(function () {
						return !testObject.pressed;
					});
			}
			function test_tintIcon() {
				verify(!testObject.tintIcon, "Initial tintIcon: false");
				testObject.tintIcon = true;
				verify(testObject.tintIcon, "tintIcon: true");
				testObject.tintIcon = false;
				verify(!testObject.tintIcon, "tintIcon: false");
			}

			when: windowShown
		}
	}
}
