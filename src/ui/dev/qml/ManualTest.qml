/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

import QtQuick

QtObject {
	property string name
	property var variants: [
		{
			"label": "Default",
			"props": {}
		}
	]
	property Component view

	function getLoremIpsum() {
		return "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris.";
	}
}
