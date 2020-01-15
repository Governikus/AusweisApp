/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ChangePinModel 1.0


TutorialContent {
	id: baseItem

	signal letsGoClicked()

	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9
		text: (SettingsModel.language === "en" ?
			   //: LABEL ANDROID IOS
			   qsTr("Please exchange your") :
			   //: LABEL ANDROID IOS
			   qsTr("Before you use the online ID function please change the")
			  ) + SettingsModel.translationTrigger
		textStyle: Style.text.tutorial_header
		horizontalAlignment: Text.AlignHCenter
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}

	Column {
		width: parent.width
		spacing: Constants.component_spacing

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("5 digits long") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("transport PIN") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
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

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("with a personal") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("6 digits long PIN") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
		}

		TutorialImage {
			width: parent.width * 0.8
			source: "qrc:///images/tutorial/generated/important_pin6.svg"
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: (SettingsModel.language === "en" ?
				   //: LABEL ANDROID IOS
				   qsTr("before you use the online ID function!") :
				   //: LABEL ANDROID IOS
				   qsTr("change!")
				  ) + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("The transport PIN is sent to you by the Bundesdruckerei via mail.") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}

	Column {
		width: parent.width
		spacing: Constants.component_spacing

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("Choose for this purpose the menu entry PIN management") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header_secondary
			horizontalAlignment: Text.AlignHCenter
		}

		Item {
			height: parent.width * 0.6
			width: parent.width

			GText {
				width: parent.width * 0.35
				//: LABEL ANDROID IOS
				text: qsTr("Later you can also change your personal PIN here") + SettingsModel.translationTrigger
				textStyle: Style.text.tutorial_content
				horizontalAlignment: Text.AlignLeft

				x: (parent.width * 0.2) - (width / 2)
				y: (parent.height * 0.5) - (height / 2)
			}

			TutorialImage {
				id: screenshot
				//: LABEL ANDROID IOS
				source: qsTr("qrc:///images/tutorial/screenshot_pin_management_menu_%1_en.png").arg(Constants.layout) + SettingsModel.translationTrigger
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
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("... or click this button to change your PIN right now:") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header_secondary
			horizontalAlignment: Text.AlignHCenter
		}

		GButton {
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.margins: Constants.component_spacing
			//: LABEL ANDROID IOS
			text: qsTr("Change PIN") + SettingsModel.translationTrigger
			onClicked: {
				NumberModel.requestTransportPin = true
				ChangePinModel.startWorkflow()
			}
		}
	}

	GText {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter

		//: LABEL ANDROID IOS
		text: qsTr("Please note: The transport PIN can only be used for your first PIN change. If you have already set your personal PIN (e.g. while picking up your ID card) only the set PIN is valid.") + SettingsModel.translationTrigger
		textStyle: Style.text.tutorial_header_secondary
		horizontalAlignment: Text.AlignHCenter
	}

	GText {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter

		//: LABEL ANDROID IOS
		text: qsTr("You can always set a new PIN at the issuing authority if the (transport) PIN is not known.") + SettingsModel.translationTrigger
		textStyle: Style.text.tutorial_header_secondary
		horizontalAlignment: Text.AlignHCenter
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

				//: LABEL ANDROID IOS
				onClicked: Qt.openUrlExternally(qsTr("https://www.youtube.com/watch?v=wZglRda5Y60&index=4&list=PLLB5ERhVkn25qQXgMHQr-1KgyZsJKoSAm"))
			}
		}

		GText {
			width: parent.width * 0.9
			anchors.horizontalCenter: parent.horizontalCenter

			//: LABEL ANDROID IOS
			text: qsTr("Learn more about this in the YouTube video") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header_secondary
			horizontalAlignment: Text.AlignHCenter
		}

		Item {
			width: parent.width
			height: letsGoImage.height

			MouseArea {
				anchors.fill: parent
				onClicked: baseItem.letsGoClicked()
			}

			TutorialImage {
				id: letsGoImage
				width: parent.width
				source: "qrc:///images/tutorial/generated/important_lets_go.svg"

				centerX: 0.5
				centerY: 0.5
			}

			GText {
				width: parent.width
				//: LABEL ANDROID IOS
				text: qsTr("Let's go") + SettingsModel.translationTrigger
				horizontalAlignment: Text.AlignHCenter
				textStyle: Style.text.tutorial_header
				color: Constants.white

				x: (parent.width * 0.5) - (width / 2)
				y: (parent.height * 0.6) - (height / 2)
			}
		}


		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("Do you still have questions?") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
		}

		Image {
			source: "qrc:///images/tutorial/generated/important_space_questionmark.svg"
			anchors.horizontalCenter: parent.horizontalCenter

			width: parent.width * 0.4
			height: width
			fillMode: Image.PreserveAspectFit
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("You can read our <b>FAQs</b> or <b>write</b> to us...") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: "<a href=\"https://%1\">%1</a><br>%2<br><a href=\"https://%3\">%3</a>".arg(
					  //: LABEL ANDROID IOS
					  qsTr("www.ausweisapp.bund.de")
				  ).arg(
					  //: LABEL ANDROID IOS
					  qsTr("or")
				  ).arg(
					  //: LABEL ANDROID IOS
					  qsTr("www.personalausweisportal.de")
				  ) + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header_secondary
			horizontalAlignment: Text.AlignHCenter
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}

	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9
		text: (Constants.is_layout_ios ?
			   //: LABEL IOS
			   qsTr("You can always access this tutorial again from the \"More\" section in the menu bar.") :
			   //: LABEL ANDROID
			   qsTr("You can always access this tutorial again from the \"Help & Feedback\" section in the side bar.")
			  ) + SettingsModel.translationTrigger
		textStyle: Style.text.tutorial_header
		horizontalAlignment: Text.AlignHCenter
	}
}
