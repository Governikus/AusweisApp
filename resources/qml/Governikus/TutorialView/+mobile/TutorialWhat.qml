/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


TutorialContent {
	id: baseItem

	Column {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter

		GText {
			width: parent.width
			//: LABEL ANDROID IOS
			text: qsTr("What is the online ID function?") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			font.italic: true
			horizontalAlignment: Text.AlignHCenter
		}
		GText {
			width: parent.width
			//: LABEL ANDROID IOS
			text: qsTr("You can use it to authenticate yourself in the internet") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
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
				color: Style.color.tutorial_box_background

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

		GText {
			width: parent.width * 0.9
			anchors.horizontalCenter: parent.horizontalCenter
			//: LABEL ANDROID IOS
			text: qsTr("and also to deal with administrative paperwork and business matters electronically!") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_content
			horizontalAlignment: Text.AlignHCenter
		}
	}

	TutorialSpacer {
		width: parent.width
		//: LABEL ANDROID IOS
		text: qsTr("Alright, but is it secure?") + SettingsModel.translationTrigger
		color: Style.color.tutorial_what
	}

	Column {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter

		GText {
			width: parent.width
			//: LABEL ANDROID IOS
			text: qsTr("Of course, because we use a so called") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_content
			horizontalAlignment: Text.AlignHCenter
		}
		GText {
			width: parent.width
			//: LABEL ANDROID IOS
			text: qsTr("Mutual authentication") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
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

		GText {
			width: parent.width * 0.9
			anchors.horizontalCenter: parent.horizontalCenter
			//: LABEL ANDROID IOS
			text: qsTr("... it establishes a secure connection between ID document and service provider.") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header_secondary
			horizontalAlignment: Text.AlignHCenter
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

			GText {
				width: parent.width * 0.35
				//: LABEL ANDROID IOS
				text: qsTr("On every authentication you get displayed <b>who</b> wants to access <b>which</b> data") + SettingsModel.translationTrigger
				textStyle: Style.text.tutorial_content
				horizontalAlignment: Text.AlignLeft

				x: (parent.width * 0.2) - (width / 2)
				y: (parent.height * 0.5) - (height / 2)
			}

			TutorialImage {
				id: screenshot
				//: LABEL ANDROID IOS
				source: qsTr("qrc:///images/tutorial/screenshot_cert_%1_en.png").arg(Constants.layout) + SettingsModel.translationTrigger
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

		GText {
			width: parent.width * 0.95
			anchors.horizontalCenter: parent.horizontalCenter
			//: LABEL ANDROID IOS
			text: qsTr("and you consent to the request with your personal PIN.") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_content
			horizontalAlignment: Text.AlignHCenter
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
				color: Style.color.tutorial_box_background
				z: 1

				x: (parent.width * 0.5) - (width / 2)
			}

			GText {
				width: parent.width
				//: LABEL ANDROID IOS
				text: qsTr("... is the provider authorized for this?") + SettingsModel.translationTrigger
				textStyle: Style.text.tutorial_header
				font.bold: true
				horizontalAlignment: Text.AlignHCenter
				z: 2

				x: (parent.width * 0.5) - (width / 2)
				y: (parent.height * 0.3) - (height / 2)
			}

			GText {
				width: parent.width - 2 * Constants.component_spacing
				//: LABEL ANDROID IOS
				text: qsTr("The provider needs an authorization of the Federal Office of Administration.") + SettingsModel.translationTrigger
				textStyle: Style.text.tutorial_content
				horizontalAlignment: Text.AlignHCenter
				z: 2

				x: (parent.width * 0.5) - (width / 2)
				y: (parent.height * 0.5)
			}
		}

		Item {
			width: parent.width
			height: parent.width * 0.3

			GText {
				width: parent.width
				//: LABEL ANDROID IOS
				text: qsTr("Certificate") + SettingsModel.translationTrigger
				textStyle: Style.text.tutorial_header
				font.bold: true
				horizontalAlignment: Text.AlignHCenter
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

		GText {
			width: parent.width * 0.9
			anchors.horizontalCenter: parent.horizontalCenter
			//: LABEL ANDROID IOS
			text: qsTr("Everytime both participants authenticate each other...") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			z: 1
		}
	}

	TutorialSpacer {
		width: parent.width
		//: LABEL ANDROID IOS
		text: qsTr("... and therefore your data is protected and securely transfered.") + SettingsModel.translationTrigger
		color: Style.color.tutorial_what
	}

	Column {
		width: parent.width
		spacing: Constants.component_spacing

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.8
			//: LABEL ANDROID IOS
			text: qsTr("You can also watch a video on YouTube on this topic") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header_secondary
			horizontalAlignment: Text.AlignHCenter
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

				//: LABEL ANDROID IOS
				onClicked: Qt.openUrlExternally(qsTr("https://www.youtube.com/watch?v=fzbUZmHaZp4&index=5&list=PLLB5ERhVkn25qQXgMHQr-1KgyZsJKoSAm"))
			}
		}
	}
}
