/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Global
import Governikus.View

ManualTest {
	id: root

	name: "FlickableSectionPage a11y scroll/swipe"
	variants: [
		{
			"label": "on button press: scroll down",
			"props": {
				"scrollDown": true
			}
		},
		{
			"label": "on button press: scroll up",
			"props": {
				"scrollDown": false
			}
		}
	]

	view: Component {
		FlickableSectionPage {
			id: flickable

			property bool scrollDown: false

			spacing: 10
			title: root.name

			Repeater {
				Layout.fillWidth: true
				model: 20

				Button {
					required property int index

					Accessible.focusable: true
					Layout.preferredHeight: 100
					text: "Button " + index

					onActiveFocusChanged: if (activeFocus)
						Utils.positionViewAtItem(this)
					onPressed: flickable.scrollDown ? flickable.scrollPageDown() : flickable.scrollPageUp()
				}
			}
		}
	}
}
