/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	id: baseItem

	property alias deleteEnabled: deleteButton.enabled
	property alias submitEnabled: submitButton.enabled

	signal digitPressed(string digit)
	signal deletePressed()
	signal submitPressed()

	QtObject {
		id: d

		property var numbers

		Component.onCompleted:  orderNumbers()

		function orderNumbers() {
			if (SettingsModel.shuffleScreenKeyboard) {
				var newNumbers = []
				for (var i = 0; i < 10; i++) {
					var random = Math.floor(Math.random() * (newNumbers.length + 1))
					newNumbers.splice(random, 0, i);
				}
				numbers = newNumbers
			}
			else {
				numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0]
			}
		}
	}

	NumberPadTab {
		id: numberPadTab

		height: ApplicationModel.scaleFactor * 100

		anchors.top: numPadContainer.top
		anchors.right: numPadContainer.left
		anchors.rightMargin: -radius

		activeFocusOnTab: true
		Accessible.role: Accessible.Grouping
		Accessible.name: qsTr("Numberpad to enter the password") + SettingsModel.translationTrigger
		Keys.onSpacePressed: numPadContainer.toggle()

		MouseArea {
			anchors.fill: parent
			onClicked: numPadContainer.toggle()
		}
	}

	Rectangle {
		id: numPadContainer

		property bool fadeIn: true

		width: grid.width + 2 * Constants.pane_padding
		height: grid.height + 2 * Constants.pane_padding

		anchors.right: parent.right
		anchors.rightMargin: fadeIn ? 0 : -width
		anchors.bottom: parent.bottom

		color: Qt.lighter(Constants.blue, 1.15)
		border.width: Math.max(1, ApplicationModel.scaleFactor * 3)
		border.color: Constants.blue

		Component.onCompleted: init()
		onVisibleChanged: init()

		function init() {
			setVisible(SettingsModel.useScreenKeyboard)
		}

		function toggle() {
			if (!fadingAnimation.running) {
				setVisible(!fadeIn)
			}
		}

		function setVisible(pVisible) {
			if (pVisible) {
				d.orderNumbers()
			}
			fadeIn = pVisible
		}

		Behavior on anchors.rightMargin {
			PropertyAnimation {
				id: fadingAnimation
				duration: 1000
				easing.type: Easing.InOutQuad
			}
		}

		Grid {
			id: grid

			anchors.centerIn: parent

			columns: 3
			spacing: Constants.component_spacing

			Repeater {
				id: numberRepeater

				model: 9

				NumberPadButton {
					activeFocusOnTab: numPadContainer.fadeIn

					text: d.numbers[index]
					onClicked: baseItem.digitPressed(text)
				}
			}

			NumberPadButton {
				id: deleteButton

				activeFocusOnTab: numPadContainer.fadeIn

				text: "C"
				color: "yellow"
				fontScale: 0.75
				onClicked: baseItem.deletePressed()
				enabled: baseItem.deleteEnabled
			}

			NumberPadButton {
				id: zeroButton

				activeFocusOnTab: numPadContainer.fadeIn

				text: d.numbers[9]
				onClicked: baseItem.digitPressed(text)
			}

			NumberPadButton {
				id: submitButton

				activeFocusOnTab: numPadContainer.fadeIn

				text: "OK"
				color: "lime"
				fontScale: 0.75
				onClicked: baseItem.submitPressed()
				enabled: baseItem.submitEnabled
			}
		}
	}
}
