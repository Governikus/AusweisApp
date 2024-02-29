/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import QtQuick
import QtTest
import Governikus.Global
import Governikus.Style

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import Governikus.Global
			GButton {}
		", testCase);
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
		let button = createTemporaryQmlObject("
			import Governikus.Global
			GButton {
				icon.source: \"" + data.icon + "\"
				text: \"" + data.text + "\"
			}
		", testCase);
		tryCompare(button, "height", data.height);
		tryCompare(button, "width", data.width);
		let buttonInLayout = createTemporaryQmlObject("
			import Governikus.Global
			import QtQuick.Layouts
			ColumnLayout {
				readonly property alias buttonHeight: mybutton.height
				readonly property alias buttonWidth: mybutton.width
				width: 1000
				GButton {
					id: mybutton
					icon.source: \"" + data.icon + "\"
					text: \"" + data.text + "\"
				}
			}
		", testCase);
		tryCompare(buttonInLayout, "buttonHeight", data.height);
		tryCompare(buttonInLayout, "buttonWidth", data.width);
		let buttonSmallLayout = createTemporaryQmlObject("
			import Governikus.Global
			import QtQuick.Layouts
			ColumnLayout {
				readonly property alias buttonHeight: mybutton.height
				readonly property alias buttonWidth: mybutton.width
				width: 75
				GButton {
					id: mybutton
					icon.source: \"" + data.icon + "\"
					text: \"" + data.text + "\"
				}
			}
		", testCase);
		tryCompare(buttonSmallLayout, "buttonHeight", data.height);
		tryCompare(buttonSmallLayout, "buttonWidth", Math.min(data.width, Constants.is_desktop ? 132 : 104));
	}
	function test_size_data() {
		let longText = createTemporaryQmlObject("import Governikus.Global; import Governikus.Style; GText {textStyle: Style.text.button; text: \"test test test test test test\"}", testCase);
		verify(waitForRendering(longText));
		let longTextWidth = Math.ceil(longText.width);
		return [{
				"tag": "noIconNoText",
				"icon": "",
				"text": "",
				"height": Constants.is_desktop ? 40 : 39,
				"width": Constants.is_desktop ? 132 : 104
			}, {
				"tag": "noIconSmallText",
				"icon": "",
				"text": "t",
				"height": Constants.is_desktop ? 40 : 39,
				"width": Constants.is_desktop ? 132 : 104
			}, {
				"tag": "noIconLongText",
				"icon": "",
				"text": "test test test test test test",
				"height": Constants.is_desktop ? 40 : 39,
				"width": (Constants.is_desktop ? 36 : 40) + longTextWidth
			}, {
				"tag": "withIconNoText",
				"icon": "qrc:///images/npa.svg",
				"text": "",
				"height": Constants.is_desktop ? 40 : 39,
				"width": Constants.is_desktop ? 132 : 104
			}, {
				"tag": "withIconSmallText",
				"icon": "qrc:///images/npa.svg",
				"text": "t",
				"height": Constants.is_desktop ? 40 : 39,
				"width": Constants.is_desktop ? 132 : 104
			}, {
				"tag": "withIconLongText",
				"icon": "qrc:///images/npa.svg",
				"text": "test test test test test test",
				"height": Constants.is_desktop ? 40 : 39,
				"width": 68 + longTextWidth
			}];
	}
	function test_size_withoutBackground(data) {
		let button = createTemporaryQmlObject("
			import Governikus.Global
			GButton {
				icon.source: \"" + data.icon + "\"
				text: \"" + data.text + "\"
				background: null
			}
		", testCase);
		tryCompare(button, "height", data.height);
		tryCompare(button, "width", data.width);
		let buttonInLayout = createTemporaryQmlObject("
			import Governikus.Global
			import QtQuick.Layouts
			ColumnLayout {
				readonly property alias buttonHeight: mybutton.height
				readonly property alias buttonWidth: mybutton.width
				width: 1000
				GButton {
					id: mybutton
					icon.source: \"" + data.icon + "\"
					text: \"" + data.text + "\"
					background: null
				}
			}
		", testCase);
		tryCompare(buttonInLayout, "buttonHeight", data.height);
		tryCompare(buttonInLayout, "buttonWidth", data.width);
		let buttonSmallLayout = createTemporaryQmlObject("
			import Governikus.Global
			import QtQuick.Layouts
			ColumnLayout {
				readonly property alias buttonHeight: mybutton.height
				readonly property alias buttonWidth: mybutton.width
				width: 75
				GButton {
					id: mybutton
					icon.source: \"" + data.icon + "\"
					text: \"" + data.text + "\"
					background: null
				}
			}
		", testCase);
		tryCompare(buttonSmallLayout, "buttonHeight", data.height);
		tryCompare(buttonSmallLayout, "buttonWidth", Math.min(data.width, 75));
	}
	function test_size_withoutBackground_data() {
		let longText = createTemporaryQmlObject("import Governikus.Global; import Governikus.Style; GText {textStyle: Style.text.button; text: \"test test test test test test\"}", testCase);
		verify(waitForRendering(longText));
		let longTextWidth = Math.ceil(longText.width);
		let smallText = createTemporaryQmlObject("import Governikus.Global; import Governikus.Style; GText {textStyle: Style.text.button; text: \"t\"}", testCase);
		verify(waitForRendering(smallText));
		let smallTextWidth = Math.ceil(smallText.width);
		return [{
				"tag": "noIconNoText",
				"icon": "",
				"text": "",
				"height": Constants.is_desktop ? 40 : 39,
				"width": Constants.is_desktop ? 36 : 40
			}, {
				"tag": "noIconSmallText",
				"icon": "",
				"text": "t",
				"height": Constants.is_desktop ? 40 : 39,
				"width": (Constants.is_desktop ? 36 : 40) + smallTextWidth
			}, {
				"tag": "noIconLongText",
				"icon": "",
				"text": "test test test test test test",
				"height": Constants.is_desktop ? 40 : 39,
				"width": (Constants.is_desktop ? 36 : 40) + longTextWidth
			}, {
				"tag": "withIconNoText",
				"icon": "qrc:///images/npa.svg",
				"text": "",
				"height": Constants.is_desktop ? 40 : 39,
				"width": 68
			}, {
				"tag": "withIconSmallText",
				"icon": "qrc:///images/npa.svg",
				"text": "t",
				"height": Constants.is_desktop ? 40 : 39,
				"width": 68 + smallTextWidth
			}, {
				"tag": "withIconLongText",
				"icon": "qrc:///images/npa.svg",
				"text": "test test test test test test",
				"height": Constants.is_desktop ? 40 : 39,
				"width": 68 + longTextWidth
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
		testObject.textStyle = Style.text.normal_warning;
		compare(testObject.textStyle, Style.text.normal_warning, "textStyle: hint_warning");
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
