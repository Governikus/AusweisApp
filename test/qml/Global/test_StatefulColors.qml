/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global
import Governikus.Type

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

		function test_checked() {
			control.enabled = true;
			control.pressed = false;
			control.hovered = false;
			control.checked = true;
			compare(colors.state, "checked");
		}
		function test_disabled(data) {
			control.enabled = false;
			control.pressed = data.pressed;
			control.hovered = data.hovered;
			control.checked = data.checked;
			compare(colors.state, "disabled");
		}
		function test_disabled_data() {
			return [
				{
					"tag": "1",
					"hovered": false,
					"pressed": false,
					"checked": false
				},
				{
					"tag": "2",
					"hovered": true,
					"pressed": false,
					"checked": false
				},
				{
					"tag": "3",
					"hovered": false,
					"pressed": true,
					"checked": false
				},
				{
					"tag": "4",
					"hovered": false,
					"pressed": false,
					"checked": true
				},
			];
		}
		function test_hovered(data) {
			let desktopOrChromeOS = Constants.is_desktop || UiPluginModel.isChromeOS;
			control.enabled = true;
			control.pressed = false;
			control.hovered = desktopOrChromeOS;
			control.checked = data.checked;
			colors.groupMember = data.groupMember;
			if (desktopOrChromeOS) {
				compare(colors.state, data.state);
			} else {
				verify(colors.state !== "hovered");
			}
		}
		function test_hovered_data() {
			return [
				{
					"tag": "1",
					"checked": false,
					"groupMember": false,
					"state": "hovered"
				},
				{
					"tag": "2",
					"checked": true,
					"groupMember": false,
					"state": "hovered"
				},
				{
					"tag": "3",
					"checked": false,
					"groupMember": true,
					"state": "hovered"
				},
				{
					"tag": "4",
					"checked": true,
					"groupMember": true,
					"state": "checked"
				}
			];
		}
		function test_pressed(data) {
			control.enabled = true;
			control.pressed = true;
			control.hovered = data.hovered;
			control.checked = data.checked;
			colors.groupMember = data.groupMember;
			compare(colors.state, data.state);
		}
		function test_pressed_data() {
			return [
				{
					"tag": "1",
					"hovered": false,
					"checked": false,
					"groupMember": false,
					"state": "pressed"
				},
				{
					"tag": "2",
					"hovered": true,
					"checked": false,
					"groupMember": false,
					"state": "pressed"
				},
				{
					"tag": "3",
					"hovered": false,
					"checked": true,
					"groupMember": false,
					"state": "pressed"
				},
				{
					"tag": "4",
					"hovered": false,
					"checked": false,
					"groupMember": true,
					"state": "pressed"
				},
				{
					"tag": "5",
					"hovered": true,
					"checked": false,
					"groupMember": true,
					"state": "pressed"
				},
				{
					"tag": "6",
					"hovered": false,
					"checked": true,
					"groupMember": true,
					"state": "checked"
				}
			];
		}

		name: "test_StatefulColors"
		visible: true
		when: windowShown
	}
}
