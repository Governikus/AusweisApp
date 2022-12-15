/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	id: baseItem

	property alias backRotation: backToMenu.imageRotation
	property alias backText: backToMenu.text
	property bool backToMenuActive: true

	signal menuClicked
	signal quitTutorialClicked

	color: footer.color
	height: plugin.safeAreaMargins.bottom + buttonRow.height
	state: "showOnlyQuit"

	Behavior on color  {
		ColorAnimation {
			duration: Constants.animation_duration
		}
	}
	states: [
		State {
			name: "showBothOptions"
			when: baseItem.backToMenuActive

			PropertyChanges {
				opacity: 1
				target: backToMenu
			}
			PropertyChanges {
				Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
				target: quitTutorial
			}
		},
		State {
			name: "showOnlyQuit"
			when: !baseItem.backToMenuActive

			PropertyChanges {
				opacity: 0
				target: backToMenu
			}
			PropertyChanges {
				Layout.alignment: Qt.AlignCenter
				target: quitTutorial
			}
		}
	]
	transitions: [
		Transition {
			PropertyAnimation {
				duration: 500
				property: "opacity"
				target: backToMenu
			}
			PropertyAnimation {
				duration: 500
				easing.type: Easing.InOutQuad
				property: "Layout.alignment"
				target: quitTutorial
			}
		}
	]

	onVisibleChanged: if (visible) {
		// When the user quits the tutorial on iOS while we're in the "showBothOptions" state, the
		// animation will finish when the MoreView is already active. This results in quitTutorial
		// being wrongly positioned the next time the users enters the tutorial. The following lines
		// work around this issue:
		if (state === "showOnlyQuit") {
			quitTutorial.anchors.horizontalCenter = baseItem.horizontalCenter;
		}
	}

	RowLayout {
		id: buttonRow
		spacing: Constants.component_spacing

		anchors {
			left: parent.left
			leftMargin: Constants.component_spacing
			right: parent.right
			rightMargin: Constants.component_spacing
			top: parent.top
		}
		TutorialFooterButton {
			id: backToMenu
			Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
			Layout.fillWidth: true
			Layout.maximumWidth: implicitWidth * opacity
			direction: Qt.LeftToRight
			imageRotation: -90
			imageSource: "qrc:///images/tutorial/arrows.svg"

			//: LABEL ANDROID IOS
			text: qsTr("Fold in")
			visible: opacity > 0.0

			onClicked: baseItem.menuClicked()
		}
		TutorialFooterButton {
			id: quitTutorial
			Layout.fillWidth: true
			Layout.maximumWidth: implicitWidth
			direction: Qt.RightToLeft
			imageSource: "qrc:///images/tutorial/cross.svg"

			//: LABEL ANDROID IOS
			text: qsTr("Quit tutorial")

			onClicked: baseItem.quitTutorialClicked()
		}
	}
}
