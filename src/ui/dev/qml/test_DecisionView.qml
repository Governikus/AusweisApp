/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

import QtQuick

import Governikus.Global

ManualTest {
	id: root

	name: "DecisionView"
	variants: [
		{
			"label": "Standard",
			"props": {
				"primaryButton.text": "primary",
				"secondaryButton.text": "secondary"
			}
		},
		{
			"label": "secondary long, primary short",
			"props": {
				"primaryButton.text": "short",
				"secondaryButton.text": "longlonglong long"
			}
		}
	]

	view: Component {
		DecisionView {
			descriptionTextsModel: [root.getLoremIpsum(), root.getLoremIpsum()]
			headlineText: "headlineText"
			secondaryButton.text: "secondary"
			subtitleText: "subtitleText"
			title: root.name

			primaryButton {
				icon.source: "qrc:///images/material_arrow_right.svg"
				layoutDirection: Qt.RightToLeft
				text: "primaryButton.Text"
				tintIcon: true
			}
		}
	}
}
