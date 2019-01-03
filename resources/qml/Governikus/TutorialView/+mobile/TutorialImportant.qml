import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ChangePinModel 1.0

TutorialContent {
	id: baseItem

	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9
		text: (settingsModel.language === "en" ? qsTr("Please exchange your") : qsTr("Before you use the online ID function please change the")) + settingsModel.translationTrigger
		font.family: "Noto Serif"
		font.pixelSize: Constants.tutorial_content_header_h1_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}

	Column {
		width: parent.width
		spacing: Constants.component_spacing

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("5 digits long") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("transport PIN") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.bold: true
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		TutorialImage {
			width: parent.width * 0.8
			source: "qrc:///images/tutorial/generated/important_pin5.svg"
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}

	Column {
		width: parent.width
		spacing: Constants.component_spacing

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("with a personal") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("6 digits long PIN") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.bold: true
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		TutorialImage {
			width: parent.width * 0.8
			source: "qrc:///images/tutorial/generated/important_pin6.svg"
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: (settingsModel.language === "en" ? qsTr("before you use the online ID function!") : qsTr("change!")) + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("The transport PIN is send to you by the Bundesdruckerei via mail.") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}

	Column {
		width: parent.width
		spacing: Constants.component_spacing

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("Choose for this purpose the menu entry PIN management") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h2_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		Item {
			height: parent.width * 0.6
			width: parent.width

			Text {
				width: parent.width * 0.35
				text: qsTr("Later you can also change your personal PIN here") + settingsModel.translationTrigger
				font.family: "Noto Serif"
				font.pixelSize: Constants.tutorial_content_font_size
				horizontalAlignment: Text.AlignLeft
				wrapMode: Text.WordWrap

				x: (parent.width * 0.2) - (width / 2)
				y: (parent.height * 0.5) - (height / 2)
			}

			TutorialImage {
				id: screenshot
				source: qsTr("qrc:///images/tutorial/screenshot_pin_management_menu_en.png") + settingsModel.translationTrigger
				z: 3

				readonly property real rightX: x + width

				width: parent.height * 0.6
				centerX: 0.6
				centerY: 0.5
			}

			Image {
				source: "qrc:///images/tutorial/zoom_triangle.svg"
				z: 4

				width: small_smartphone.centerXValue - screenshot.rightX
				height: screenshot.height
				x: screenshot.rightX
				y: (parent.height * 0.5) - (height / 2)
			}

			TutorialImage {
				id: small_smartphone
				source: "qrc:///images/tutorial/phone.svg"
				z: 1

				readonly property real centerXValue: x + (width / 2)

				width: parent.height * 0.2
				centerY: 0.5
				centerX: 0.9
			}
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("... or click this button to change your PIN right now:") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h2_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		GButton {
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.margins: Constants.component_spacing
			text: qsTr("Change PIN") + settingsModel.translationTrigger
			onClicked: {
				NumberModel.requestTransportPin = true
				ChangePinModel.startWorkflow()
			}
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}

	Column {
		width: parent.width
		spacing: Constants.component_spacing

		Image {
			anchors.horizontalCenter: parent.horizontalCenter
			source: "qrc:///images/tutorial/play_movie.png"
			width: parent.width * 0.7
			height: width * (sourceSize.height / sourceSize.width)
			fillMode: Image.PreserveAspectFit

			MouseArea {
				anchors.fill: parent

				onClicked: Qt.openUrlExternally(qsTr("https://www.youtube.com/watch?v=wZglRda5Y60&index=4&list=PLLB5ERhVkn25qQXgMHQr-1KgyZsJKoSAm"))
			}
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("Learn more about this in the YouTube video") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h2_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		Item {
			width: parent.width
			height: letsGoImage.height

			TutorialImage {
				id: letsGoImage
				width: parent.width
				source: "qrc:///images/tutorial/generated/important_lets_go.svg"

				centerX: 0.5
				centerY: 0.5
			}

			Text {
				width: parent.width
				text: qsTr("Let's go") + settingsModel.translationTrigger
				horizontalAlignment: Text.AlignHCenter
				wrapMode: Text.WordWrap
				font.family: "Noto Serif"
				font.pixelSize: Constants.tutorial_content_header_h1_font_size
				color: Constants.white

				x: (parent.width * 0.5) - (width / 2)
				y: (parent.height * 0.6) - (height / 2)
			}
		}


		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("Do you still have questions?") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		Image {
			source: "qrc:///images/tutorial/generated/important_space_questionmark.svg"
			anchors.horizontalCenter: parent.horizontalCenter

			width: parent.width * 0.4
			height: width
			fillMode: Image.PreserveAspectFit
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("You can read our <b>FAQs</b> or <b>write</b> to us...") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: "<a href=\"https://%1\">%1</a><br>%2<br><a href=\"https://%3\">%3</a>".arg(qsTr("www.ausweisapp.bund.de")).arg(qsTr("or")).arg(qsTr("www.personalausweisportal.de")) + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h2_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.Wrap
			onLinkActivated: Qt.openUrlExternally(link)
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}

	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9
		text: qsTr("You can always access this tutorial again from the side bar.") + settingsModel.translationTrigger
		font.family: "Noto Serif"
		font.pixelSize: Constants.tutorial_content_header_h1_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
	}
}
