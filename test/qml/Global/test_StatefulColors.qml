/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

Item {
	id: control

	property bool checkable: true
	property bool checked: false
	property bool enabled: true
	property bool hovered: false
	property bool pressed: false

	StatefulColors {
		id: colors

		statefulControl: control
	}
	TestCase {
		id: testCase

		function test_disabled(data) {
			control.enabled = false;
			control.pressed = data.pressed;
			control.hovered = data.hovered;
			control.checked = data.checked;
			compare(colors.state, "disabled");
		}
		function test_disabled_data() {
			return [{
					"tag": "1",
					"hovered": false,
					"pressed": false,
					"checked": false
				}, {
					"tag": "2",
					"hovered": true,
					"pressed": false,
					"checked": false
				}, {
					"tag": "3",
					"hovered": false,
					"pressed": true,
					"checked": false
				}, {
					"tag": "4",
					"hovered": false,
					"pressed": false,
					"checked": true
				},];
		}
		function test_hovered(data) {
			control.enabled = true;
			control.pressed = false;
			control.hovered = true;
			control.checked = data.checked;
			if (Constants.is_desktop) {
				compare(colors.state, "hovered");
			} else {
				verify(colors.state !== "hovered");
			}
		}
		function test_hovered_data() {
			return [{
					"tag": "1",
					"checked": false
				}, {
					"tag": "2",
					"checked": true
				},];
		}
		function test_pressed(data) {
			control.enabled = true;
			control.pressed = true;
			control.hovered = data.hovered;
			control.checked = data.checked;
			compare(colors.state, "pressed");
		}
		function test_pressed_data() {
			return [{
					"tag": "1",
					"hovered": false,
					"checked": false
				}, {
					"tag": "2",
					"hovered": true,
					"checked": false
				}, {
					"tag": "3",
					"hovered": false,
					"checked": true
				},];
		}
		function test_unchecked() {
			control.enabled = true;
			control.pressed = false;
			control.hovered = false;
			control.checked = false;
			compare(colors.state, "unchecked");
		}

		name: "test_StatefulColors"
		visible: true
		when: windowShown
	}
}
