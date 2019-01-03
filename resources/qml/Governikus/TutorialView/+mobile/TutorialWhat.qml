import QtQuick 2.10

import Governikus.Global 1.0

TutorialContent {
	id: baseItem

	Column {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter

		Text {
			width: parent.width
			text: qsTr("What is the online ID function?") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			font.italic: true
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}
		Text {
			width: parent.width
			text: qsTr("You can authenticate yourself in the internet") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_what.svg"
	}

	Column {
		width: parent.width
		spacing: Constants.component_spacing

		Item {
			height: parent.width * 0.6
			width: parent.width

			Rectangle {
				id: circle
				width: parent.width * 0.6
				height: width
				radius: width * 0.5
				color: Constants.tutorial_very_light_grey

				x: (parent.width * 0.5) - (width / 2)
			}

			TutorialImage {
				id: stylisedEpa
				source: "qrc:///images/tutorial/idcard.svg"
				z: 2

				width: parent.height * 0.6
				centerY: 0.5
				centerX: 0.8
			}

			TutorialImage {
				id: smartPhone
				source: "qrc:///images/tutorial/phone.svg"
				z: 3

				width: parent.height * 0.4
				centerY: 0.5
				centerX: 0.2
			}

			TutorialImage {
				id: shield
				source: "qrc:///images/tutorial/save.svg"
				z: 4

				width: parent.height * 0.3
				centerY: 0.5
				centerX: 0.5
			}
		}

		Text {
			width: parent.width * 0.9
			anchors.horizontalCenter: parent.horizontalCenter
			text: qsTr("and use it to deal with administrative paperwork and business matters electronically!") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}
	}

	TutorialSpacer {
		width: parent.width
		text: qsTr("Alright, but is it secure?") + settingsModel.translationTrigger
		color: Constants.tutorial_orange
	}

	Column {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter

		Text {
			width: parent.width
			text: qsTr("Of course, because we use a so called") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}
		Text {
			width: parent.width
			text: qsTr("Mutual authentication") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_what.svg"
	}

	Column {
		width: parent.width
		spacing: Constants.component_spacing

		Item {
			height: parent.width * 0.6
			width: parent.width

			TutorialImage {
				source: "qrc:///images/tutorial/user-tine@3x.png"
				z: 2

				width: parent.height * 0.3
				centerY: 0.5
				centerX: 0.25
			}

			TutorialImage {
				source: "qrc:///images/npa.svg"
				z: 3

				width: parent.height * 0.15
				centerY: 0.65
				centerX: 0.2
			}

			TutorialImage {
				source: "qrc:///images/tutorial/circle-lock.svg"
				z: 1

				width: parent.height * 0.7
				centerY: 0.75
				centerX: 0.45
			}

			TutorialImage {
				source: "qrc:///images/tutorial/circle-lock-2.svg"
				z: 1

				width: parent.height * 0.7
				centerY: 0.2
				centerX: 0.55
			}

			TutorialImage {
				source: "qrc:///images/tutorial/providericons.png"
				z: 2

				width: parent.height * 0.8
				centerY: 0.5
				centerX: 0.7
			}

			TutorialImage {
				source: "qrc:///images/tutorial/provider_home.svg"
				z: 3

				width: parent.height * 0.18
				centerY: 0.48
				centerX: 0.7
			}
		}

		Text {
			width: parent.width * 0.9
			anchors.horizontalCenter: parent.horizontalCenter
			text: qsTr("... it establishes a secure connection between ID document and service provider.") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h2_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_what.svg"
	}


	Column {
		width: parent.width
		spacing: Constants.component_spacing * 2

		Item {
			height: parent.width * 0.6
			width: parent.width

			Text {
				width: parent.width * 0.35
				text: qsTr("On every authentication you get displayed <b>who</b> wants to access <b>which</b> data") + settingsModel.translationTrigger
				font.family: "Noto Serif"
				font.pixelSize: Constants.tutorial_content_font_size
				horizontalAlignment: Text.AlignLeft
				wrapMode: Text.WordWrap

				x: (parent.width * 0.2) - (width / 2)
				y: (parent.height * 0.5) - (height / 2)
			}

			TutorialImage {
				id: screenshot
				source: qsTr("qrc:///images/tutorial/screenshot_cert_en.png") + settingsModel.translationTrigger
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
			width: parent.width * 0.95
			anchors.horizontalCenter: parent.horizontalCenter
			text: qsTr("and you consent to the request with your personal PIN.") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		Item {
			width: parent.width
			height: parent.width * 0.3
			z: 5

			TutorialImage {
				source: "qrc:///images/tutorial/pin-6@2x.png"

				width: parent.height
				centerX: 0.6
				centerY: 0.5
			}
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_what.svg"
	}


	Column {
		width: parent.width
		spacing: Constants.component_spacing

		Item {
			width: parent.width
			height: width * 0.6

			Rectangle {
				width: parent.width * 0.6
				height: width
				radius: width * 0.5
				color: Constants.tutorial_very_light_grey
				z: 1

				x: (parent.width * 0.5) - (width / 2)
			}

			Text {
				width: parent.width
				text: qsTr("... is the provider authorized for this?") + settingsModel.translationTrigger
				font.family: "Noto Serif"
				font.pixelSize: Constants.tutorial_content_header_h1_font_size
				font.bold: true
				horizontalAlignment: Text.AlignHCenter
				wrapMode: Text.WordWrap
				z: 2

				x: (parent.width * 0.5) - (width / 2)
				y: (parent.height * 0.3) - (height / 2)
			}

			Text {
				width: parent.width
				text: qsTr("The provider needs an authorization of the Federal Office of Administration.") + settingsModel.translationTrigger
				font.family: "Noto Serif"
				font.pixelSize: Constants.tutorial_content_font_size
				horizontalAlignment: Text.AlignHCenter
				wrapMode: Text.WordWrap
				z: 2

				x: (parent.width * 0.5) - (width / 2)
				y: (parent.height * 0.5) - (height / 2)
			}
		}

		Item {
			width: parent.width
			height: parent.width * 0.3

			Text {
				width: parent.width
				text: qsTr("Certificate") + settingsModel.translationTrigger
				font.family: "Noto Serif"
				font.pixelSize: Constants.tutorial_content_header_h1_font_size
				font.bold: true
				horizontalAlignment: Text.AlignHCenter
				wrapMode: Text.WordWrap
				z: 1

				x: (parent.width * 0.2) - (width / 2)
				y: (parent.height * 0.5) - (height / 2)
			}

			TutorialImage {
				source: "qrc:///images/tutorial/provider_home.svg"
				z: 1

				width: parent.height * 0.7
				centerY: 0.5
				centerX: 0.5
			}

			TutorialImage {
				source: "qrc:///images/tutorial/bva.svg"
				z: 1

				width: parent.height
				centerY: 0.5
				centerX: 0.8
			}
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_what.svg"
	}

	Column {
		width: parent.width
		spacing: Constants.component_spacing

		Item {
			width: parent.width
			height: parent.width * 0.3

			TutorialImage {
				source: "qrc:///images/tutorial/user-tine@3x.png"
				z: 1

				width: parent.height * 0.5
				centerY: 0.4
				centerX: 0.15
			}

			TutorialImage {
				source: "qrc:///images/tutorial/pin-6@2x.png"
				z: 2

				width: parent.height
				centerY: 0.5
				centerX: 0.25
			}

			TutorialImage {
				source: "qrc:///images/tutorial/provider_home.svg"
				z: 1

				width: parent.height * 0.6
				centerY: 0.25
				centerX: 0.7
			}

			TutorialImage {
				source: "qrc:///images/tutorial/bva.svg"
				z: 2

				width: parent.height * 0.8
				centerY: 0.5
				centerX: 0.8
			}
		}

		Text {
			width: parent.width * 0.9
			anchors.horizontalCenter: parent.horizontalCenter
			text: qsTr("Everytime both participants authenticate each other...") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
			z: 1
		}
	}

	TutorialSpacer {
		width: parent.width
		text: qsTr("... and therefore your data is protected and securely transfered.") + settingsModel.translationTrigger
		color: Constants.tutorial_orange
	}

	Column {
		width: parent.width
		spacing: Constants.component_spacing

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.8
			text: qsTr("You can also watch a video on YouTube on this topic") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h2_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
			z: 2
		}

		Image {
			anchors.horizontalCenter: parent.horizontalCenter
			source: "qrc:///images/tutorial/play_movie.png"
			width: parent.width * 0.7
			height: width * (sourceSize.height / sourceSize.width)
			fillMode: Image.PreserveAspectFit

			MouseArea {
				anchors.fill: parent

				onClicked: Qt.openUrlExternally(qsTr("https://www.youtube.com/watch?v=fzbUZmHaZp4&index=5&list=PLLB5ERhVkn25qQXgMHQr-1KgyZsJKoSAm"))
			}
		}
	}
}
