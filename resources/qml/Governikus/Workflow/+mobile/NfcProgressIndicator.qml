import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Global 1.0


Item {
	id: baseItem

	SequentialAnimation {
		id: shaking
		loops: Animation.Infinite
		readonly property int delta: Utils.dp(4)
		readonly property int deltaDuration: 300

		ParallelAnimation {
			SequentialAnimation {
				PropertyAnimation { target: phone.anchors; property: "horizontalCenterOffset"; to: -shaking.delta; duration: shaking.deltaDuration }
				PropertyAnimation { target: phone.anchors; property: "horizontalCenterOffset"; to: shaking.delta; duration: 2*shaking.deltaDuration }
				PropertyAnimation { target: phone.anchors; property: "horizontalCenterOffset"; to: 0; duration: shaking.deltaDuration }
			}
			SequentialAnimation {
				PropertyAnimation { target: phone.anchors; property: "verticalCenterOffset"; to: -shaking.delta; duration: 2*shaking.deltaDuration }
				PropertyAnimation { target: phone.anchors; property: "verticalCenterOffset"; to: 0; duration: 2*shaking.deltaDuration }
			}
		}
		ParallelAnimation {
			SequentialAnimation {
				PropertyAnimation { target: phone.anchors; property: "horizontalCenterOffset"; to: -shaking.delta; duration: shaking.deltaDuration }
				PropertyAnimation { target: phone.anchors; property: "horizontalCenterOffset"; to: shaking.delta; duration: 2*shaking.deltaDuration }
				PropertyAnimation { target: phone.anchors; property: "horizontalCenterOffset"; to: 0; duration: shaking.deltaDuration }
			}
			SequentialAnimation {
				PropertyAnimation { target: phone.anchors; property: "verticalCenterOffset"; to: shaking.delta; duration: 2*shaking.deltaDuration }
				PropertyAnimation { target: phone.anchors; property: "verticalCenterOffset"; to: 0; duration: 2*shaking.deltaDuration }
			}
		}
	}

	states: [
		State {
			name: "off"
			PropertyChanges { target: grayLevel; visible: true }
			PropertyChanges { target: card; x: baseItem.width; y: baseItem.height / 2; rotation: -180 }
			PropertyChanges { target: shaking; running: false }
		},
		State {
			name: "on"
			PropertyChanges { target: grayLevel; visible: false }
			PropertyChanges { target: card; x: (baseItem.width + phone.width - card.width)/2; y: baseItem.height / 4; rotation: 0 }
			PropertyChanges { target: shaking; running: true }
		}
	]

	transitions:
		Transition {
			from: "off"; to: "on"; reversible: false
			SequentialAnimation
			{
				PauseAnimation { duration: 500 }
				PropertyAnimation{ targets: card; properties: "x,y,rotation"; duration: 500; easing.type: Easing.OutQuad }
				PropertyAnimation{ targets: phone; property: "x"; duration: 500; easing.type: Easing.OutQuart }
				PropertyAction { target: shaking; property: "running" }
			}
		}

	Image {
		id: card
		source: "qrc:///images/ausweis.png"
		height: phone.height
		width: height
		fillMode: Image.PreserveAspectFit
	}

	Image {
		id: phone
		source: "qrc:///images/phone_nfc.svg"
		height: parent.height * 0.5
		width: height
		anchors.centerIn: parent
		fillMode: Image.PreserveAspectFit
	}
	Colorize {
		id: grayLevel
		source: phone
		anchors.fill: phone
		saturation: 0
		hue: 0
		lightness: 0.3
		cached: true
	}
}
