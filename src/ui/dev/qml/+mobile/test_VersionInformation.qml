/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

import QtQuick

import Governikus.InformationView

import ".."

ManualTest {
	id: root

	property ListModel modelMock: ListModel {
		ListElement {
			key: "short key"
			value: "short value"
		}
		ListElement {
			key: "real value"
			value: "SM-S901B (samsung/r0sxeea/r0s:16/BP2A.250605.031.A3/S901BXXSNGZD7:user/release-keys)"
		}
		ListElement {
			key: "non-hyphenated word"
			value: "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"
		}
	}

	name: "VersionInformation"
	variants: [
		{
			"label": "Standard",
			"props": {}
		},
		{
			"label": "Model with test entries",
			"props": {
				"versionInformationModel": modelMock
			}
		}
	]

	view: Component {
		VersionInformation {
			title: root.name
		}
	}
}
