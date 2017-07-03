import QtQuick 2.5

import "global"

Item {
	height: childrenRect.height
	width: parent.width


	SequentialAnimation {
		id: shakingAnimation
		loops: Animation.Infinite
		readonly property int delta: Utils.dp(4)
		readonly property int deltaDuration: 300

		ParallelAnimation {
			SequentialAnimation {
				PropertyAnimation { target: phoneNfcImage; property: "x"; to: phoneNfcImage.x - shakingAnimation.delta; duration: shakingAnimation.deltaDuration }
				PropertyAnimation { target: phoneNfcImage; property: "x"; to: phoneNfcImage.x + shakingAnimation.delta; duration: 2*shakingAnimation.deltaDuration }
				PropertyAnimation { target: phoneNfcImage; property: "x"; to: phoneNfcImage.x; duration: shakingAnimation.deltaDuration }
			}
			SequentialAnimation {
				PropertyAnimation { target: phoneNfcImage; property: "y"; to: phoneNfcImage.y - shakingAnimation.delta; duration: 2*shakingAnimation.deltaDuration }
				PropertyAnimation { target: phoneNfcImage; property: "y"; to: phoneNfcImage.y; duration: 2*shakingAnimation.deltaDuration }
			}
		}
		ParallelAnimation {
			SequentialAnimation {
				PropertyAnimation { target: phoneNfcImage; property: "x"; to: phoneNfcImage.x - shakingAnimation.delta; duration: shakingAnimation.deltaDuration }
				PropertyAnimation { target: phoneNfcImage; property: "x"; to: phoneNfcImage.x + shakingAnimation.delta; duration: 2*shakingAnimation.deltaDuration }
				PropertyAnimation { target: phoneNfcImage; property: "x"; to: phoneNfcImage.x; duration: shakingAnimation.deltaDuration }
			}
			SequentialAnimation {
				PropertyAnimation { target: phoneNfcImage; property: "y"; to: phoneNfcImage.y + shakingAnimation.delta; duration: 2*shakingAnimation.deltaDuration }
				PropertyAnimation { target: phoneNfcImage; property: "y"; to: phoneNfcImage.y; duration: 2*shakingAnimation.deltaDuration }
			}
		}
	}

	state: visible ? "on" : "off"
	states: [
		State {
			name: "off"
			PropertyChanges { target: cardNfcImage; x: images.width; y: images.height / 2; rotation: -180 }
			PropertyChanges { target: phoneNfcImage; x: 0 }
			PropertyChanges { target: shakingAnimation; running: false }
		},
		State {
			name: "on"
			PropertyChanges { target: cardNfcImage; x: (images.width + phoneNfcImage.width - cardNfcImage.width)/2; y:  cardNfcImage.height / 4; rotation: 0 }
			PropertyChanges { target: phoneNfcImage; x: (images.width - phoneNfcImage.width)/2 }
			PropertyChanges { target: shakingAnimation; running: true }
		}
	]

	transitions:
		Transition {
			from: "off"; to: "on"; reversible: false
			SequentialAnimation
			{
				PauseAnimation { duration: 500 }
				PropertyAnimation{ targets: cardNfcImage; properties: "x,y,rotation"; duration: 500; easing.type: Easing.OutQuad }
				PropertyAnimation{ targets: phoneNfcImage; property: "x"; duration: 500; easing.type: Easing.OutQuart }
				PropertyAction { target: shakingAnimation; property: "running" }
			}
		}

	Item {
		id: images
		width: parent.width
		height: parent.height / 8 * 3
		anchors.top: parent.top
		anchors.topMargin: parent.height / 8 * 1
		Image {
			id: cardNfcImage
			source: "qrc:///images/ausweis.png"
			height: images.height / 3 * 2 // == parent.height / 8 * 2
			width: height
			fillMode: Image.PreserveAspectFit
		}
		Image {
			id: phoneNfcImage
			source: "qrc:///images/androidtelefon.png"
			height: cardNfcImage.height
			width: height
			fillMode: Image.PreserveAspectFit

		}
	}

	Text {
		id: headerText
		anchors.top: images.bottom
		anchors.topMargin: Utils.dp(30)
		anchors.horizontalCenter: parent.horizontalCenter
		text: qsTr("Establish connection")
		font.pixelSize: Constants.header_font_size
		color: Constants.blue
	}
	Text {
		function getInfoText() {
			if (!applicationModel.nfcEnabled) {
				return qsTr("NFC not supported") + "\n" + qsTr("or switched off");
			}
			else if (applicationModel.extendedLengthApdusUnsupported) {
				return qsTr("Your phone does not support the") + "\n" + qsTr("required extended length ADPUs.");
			}

			return qsTr("Please place your smartphone") + "\n" + qsTr("on your id card");
		}

		id: infoText
		anchors.top: headerText.bottom
		anchors.topMargin: Utils.dp(30)
		anchors.horizontalCenter: parent.horizontalCenter
		width: text.width
		text:  getInfoText()
		color: applicationModel.nfcEnabled && !applicationModel.extendedLengthApdusUnsupported ? "black" : Constants.red
		font.pixelSize: Constants.normal_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
	}
}
