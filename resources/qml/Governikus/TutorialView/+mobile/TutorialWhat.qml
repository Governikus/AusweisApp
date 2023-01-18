/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

TutorialContent {
	id: baseItem
	Column {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9

		GText {
			font.italic: true
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("What is the online ID function?")
			textStyle: Style.text.tutorial_header
			width: parent.width

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
		GText {
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("You can use it to authenticate yourself in the internet")
			textStyle: Style.text.tutorial_header
			width: parent.width

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_what.svg"
	}
	Column {
		spacing: Constants.component_spacing
		width: parent.width

		Item {
			height: parent.width * 0.6
			width: parent.width

			Rectangle {
				id: circle
				color: Style.color.tutorial_box_background
				height: width
				radius: width * 0.5
				width: parent.width * 0.6
				x: (parent.width * 0.5) - (width / 2)
			}
			TutorialImage {
				id: stylisedEpa
				centerX: 0.8
				centerY: 0.5
				rotation: 12
				source: "qrc:///images/ausweis.svg"
				width: parent.height * 0.4
				z: 2
			}
			TutorialImage {
				id: smartPhone
				centerX: 0.2
				centerY: 0.5
				source: "qrc:///images/tutorial/phone.svg"
				width: parent.height * 0.4
				z: 3
			}
			TutorialImage {
				id: shield
				centerX: 0.5
				centerY: 0.5
				source: "qrc:///images/tutorial/save.svg"
				width: parent.height * 0.3
				z: 4
			}
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("and also to deal with administrative paperwork and business matters electronically!")
			textStyle: Style.text.tutorial_content
			width: parent.width * 0.9

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
	}
	TutorialSpacer {
		color: Style.color.tutorial_what

		//: LABEL ANDROID IOS
		text: qsTr("Alright, but is it secure?")
		width: parent.width

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
	}
	Column {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9

		GText {
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("Of course, because we use a so called")
			textStyle: Style.text.tutorial_content
			width: parent.width

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
		GText {
			font.bold: true
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("Mutual authentication")
			textStyle: Style.text.tutorial_header
			width: parent.width

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_what.svg"
	}
	Column {
		spacing: Constants.component_spacing
		width: parent.width

		Item {
			height: parent.width * 0.6
			width: parent.width

			TutorialImage {
				centerX: 0.25
				centerY: 0.5
				source: "qrc:///images/tutorial/user-tine@3x.png"
				width: parent.height * 0.3
				z: 2
			}
			TutorialImage {
				centerX: 0.2
				centerY: 0.65
				source: "qrc:///images/npa.svg"
				width: parent.height * 0.15
				z: 3
			}
			TutorialImage {
				centerX: 0.45
				centerY: 0.75
				source: "qrc:///images/tutorial/circle-lock.svg"
				width: parent.height * 0.7
				z: 1
			}
			TutorialImage {
				centerX: 0.55
				centerY: 0.2
				source: "qrc:///images/tutorial/circle-lock-2.svg"
				width: parent.height * 0.7
				z: 1
			}
			TutorialImage {
				centerX: 0.7
				centerY: 0.5
				source: "qrc:///images/tutorial/providericons.png"
				width: parent.height * 0.8
				z: 2
			}
			TutorialImage {
				centerX: 0.7
				centerY: 0.48
				source: "qrc:///images/tutorial/provider_home.svg"
				width: parent.height * 0.18
				z: 3
			}
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("... it establishes a secure connection between ID card and provider.")
			textStyle: Style.text.tutorial_header_secondary
			width: parent.width * 0.9

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_what.svg"
	}
	Column {
		spacing: Constants.component_spacing * 2
		width: parent.width

		Item {
			height: parent.width * 0.6
			width: parent.width

			GText {
				horizontalAlignment: Text.AlignLeft

				//: LABEL ANDROID IOS
				text: qsTr("On every authentication you get displayed <b>who</b> wants to access <b>which</b> data")
				textStyle: Style.text.tutorial_content
				width: parent.width * 0.35
				x: (parent.width * 0.2) - (width / 2)
				y: (parent.height * 0.5) - (height / 2)

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
			}
			TutorialImage {
				id: screenshot

				readonly property real rightX: x + width

				centerX: 0.6
				centerY: 0.5
				source: "qrc:///images/tutorial/screenshot_cert_%1_%2.png".arg(Constants.layout).arg(SettingsModel.language)
				width: parent.height * 0.6
				z: 3
			}
			TutorialZoomTriangle {
				height: screenshot.height
				opacity: 0.3
				width: small_smartphone.centerXValue - screenshot.rightX
				x: screenshot.rightX
				y: (parent.height * 0.5) - (height / 2)
				z: 4
			}
			TutorialImage {
				id: small_smartphone

				readonly property real centerXValue: x + (width / 2)

				centerX: 0.9
				centerY: 0.5
				source: "qrc:///images/tutorial/phone.svg"
				width: parent.height * 0.2
				z: 1
			}
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("and you consent to the request with your six-digit PIN.")
			textStyle: Style.text.tutorial_content
			width: parent.width * 0.95

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
		Item {
			height: parent.width * 0.3
			width: parent.width
			z: 5

			TutorialImage {
				centerX: 0.6
				centerY: 0.5
				source: "qrc:///images/tutorial/pin-6@2x.png"
				width: parent.height
			}
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_what.svg"
	}
	Column {
		spacing: Constants.component_spacing
		width: parent.width

		Item {
			height: width * 0.6
			width: parent.width

			Rectangle {
				color: Style.color.tutorial_box_background
				height: width
				radius: width * 0.5
				width: parent.width * 0.6
				x: (parent.width * 0.5) - (width / 2)
				z: 1
			}
			GText {
				font.bold: true
				horizontalAlignment: Text.AlignHCenter

				//: LABEL ANDROID IOS
				text: qsTr("... is the provider authorized for this?")
				textStyle: Style.text.tutorial_header
				width: parent.width - 2 * Constants.component_spacing
				x: (parent.width * 0.5) - (width / 2)
				y: (parent.height * 0.3) - (height / 2)
				z: 2

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
			}
			GText {
				horizontalAlignment: Text.AlignHCenter

				//: LABEL ANDROID IOS
				text: qsTr("The provider needs an authorization of the Federal Office of Administration.")
				textStyle: Style.text.tutorial_content
				width: parent.width - 2 * Constants.component_spacing
				x: (parent.width * 0.5) - (width / 2)
				y: (parent.height * 0.5)
				z: 2

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
			}
		}
		Item {
			height: parent.width * 0.3
			width: parent.width

			GText {
				font.bold: true
				horizontalAlignment: Text.AlignHCenter

				//: LABEL ANDROID IOS
				text: qsTr("Certificate")
				textStyle: Style.text.tutorial_header
				width: parent.width
				x: (parent.width * 0.2) - (width / 2)
				y: (parent.height * 0.5) - (height / 2)
				z: 1

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
			}
			TutorialImage {
				centerX: 0.5
				centerY: 0.5
				source: "qrc:///images/tutorial/provider_home.svg"
				width: parent.height * 0.7
				z: 1
			}
			TutorialImage {
				centerX: 0.8
				centerY: 0.5
				source: "qrc:///images/tutorial/bva.svg"
				width: parent.height
				z: 1
			}
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_what.svg"
	}
	Column {
		spacing: Constants.component_spacing
		width: parent.width

		Item {
			height: parent.width * 0.3
			width: parent.width

			TutorialImage {
				centerX: 0.15
				centerY: 0.4
				source: "qrc:///images/tutorial/user-tine@3x.png"
				width: parent.height * 0.5
				z: 1
			}
			TutorialImage {
				centerX: 0.25
				centerY: 0.5
				source: "qrc:///images/tutorial/pin-6@2x.png"
				width: parent.height
				z: 2
			}
			TutorialImage {
				centerX: 0.7
				centerY: 0.25
				source: "qrc:///images/tutorial/provider_home.svg"
				width: parent.height * 0.6
				z: 1
			}
			TutorialImage {
				centerX: 0.8
				centerY: 0.5
				source: "qrc:///images/tutorial/bva.svg"
				width: parent.height * 0.8
				z: 2
			}
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("Every time both participants authenticate each other...")
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.9
			z: 1

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
	}
	TutorialSpacer {
		color: Style.color.tutorial_what

		//: LABEL ANDROID IOS
		text: qsTr("... and therefore your data is protected and securely transferred.")
		width: parent.width

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
	}
	Column {
		spacing: Constants.component_spacing
		width: parent.width

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("You can also watch a video on YouTube on this topic")
			textStyle: Style.text.tutorial_header_secondary
			width: parent.width * 0.8
			z: 2

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
		Image {
			anchors.horizontalCenter: parent.horizontalCenter
			fillMode: Image.PreserveAspectFit
			height: width * (sourceSize.height / sourceSize.width)
			source: "qrc:///images/tutorial/play_movie.png"
			width: parent.width * 0.7

			MouseArea {

				//: LABEL ANDROID IOS
				Accessible.name: qsTr("Open YouTube video")
				Accessible.role: Accessible.Button
				anchors.fill: parent

				Accessible.onPressAction: clicked(null)
				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/video-authentication-example".arg(SettingsModel.language))
			}
		}
	}
}
