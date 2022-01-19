/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.PinResetInformationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.UiModule 1.0

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
			  )
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
			text: qsTr("five-digit")
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("Transport PIN")
			textStyle: Style.text.tutorial_header
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
		}

		TutorialImage {
			width: parent.width * 0.8
			source: "qrc:///images/tutorial/important_pin5.svg"
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
			text: qsTr("with a")
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("six-digit PIN")
			textStyle: Style.text.tutorial_header
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
		}

		TutorialImage {
			width: parent.width * 0.8
			source: "qrc:///images/tutorial/important_pin6.svg"
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: (SettingsModel.language === "en" ?
				   //: LABEL ANDROID IOS
				   qsTr("before you use the online ID function!") :
				   //: LABEL ANDROID IOS
				   qsTr("change!")
				  )
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("The Transport PIN is sent to you by the Bundesdruckerei via mail.")
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
			text: qsTr("Select for this purpose the menu item \"Change my (Transport) PIN\" from the start page. Later you can also change your six-digit PIN here")
			textStyle: Style.text.tutorial_header_secondary
			horizontalAlignment: Text.AlignHCenter
		}

		TutorialShowMenuPath {
			width: parent.width

			newSectionImage: "qrc:///images/tutorial/screenshot_pin_management_menu_%1_%2.png".arg(Constants.layout).arg(SettingsModel.language)
			newSectionPointerY: Constants.is_layout_android ? 0.47 : 0.5
			backgroundIcon: "qrc:///images/tutorial/background_icon_important.svg"
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("... or click this button to change your PIN right now:")
			textStyle: Style.text.tutorial_header_secondary
			horizontalAlignment: Text.AlignHCenter
		}

		GButton {
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.margins: Constants.component_spacing
			//: LABEL ANDROID IOS
			text: qsTr("Change my (Transport) PIN")
			onClicked: {
				SettingsModel.transportPinReminder = false
				SettingsModel.startupModule = UiModule.DEFAULT
				navBar.show(UiModule.PINMANAGEMENT)
			}
		}
	}

	GText {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter

		//: LABEL ANDROID IOS
		text: qsTr("Please note: The Transport PIN can only be used for your first PIN change. If you have already set your six-digit PIN (e.g. while picking up your ID card) only the set PIN is valid.")
		textStyle: Style.text.tutorial_header_secondary
		horizontalAlignment: Text.AlignHCenter
	}

	GText {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter

		text: PinResetInformationModel.pinForgottenTutorialHint
		textStyle: Style.text.tutorial_header_secondary
		horizontalAlignment: Text.AlignHCenter
	}

	GButton {
		anchors.horizontalCenter: parent.horizontalCenter

		visible: text !== ""

		text: PinResetInformationModel.pinResetActionText

		onClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
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
				Accessible.name: qsTr("Open YouTube video")
				Accessible.role: Accessible.Button
				Accessible.onPressAction: if (Qt.platform.os === "ios") clicked(null)

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/video-passwords".arg(SettingsModel.language))
			}
		}

		GText {
			width: parent.width * 0.9
			anchors.horizontalCenter: parent.horizontalCenter

			//: LABEL ANDROID IOS
			text: qsTr("Learn more about this in the YouTube video")
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
				source: "qrc:///images/tutorial/important_lets_go.svg"

				centerX: 0.5
				centerY: 0.5
			}

			GText {
				width: parent.width
				//: LABEL ANDROID IOS
				text: qsTr("Let's go")
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
			text: qsTr("Do you still have questions?")
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
		}

		Image {
			source: "qrc:///images/tutorial/important_space_questionmark.svg"
			anchors.horizontalCenter: parent.horizontalCenter

			width: parent.width * 0.4
			height: width
			fillMode: Image.PreserveAspectFit
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("You can read our <b>FAQs</b> or <b>write</b> to us...")
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: "<a href=\"https://%1\">%1</a><br>%2<br><a href=\"https://%3\">%3</a>".arg(
					  //: LABEL ANDROID IOS
					  qsTr("www.ausweisapp.bund.de/en")
				  ).arg(
					  //: LABEL ANDROID IOS
					  qsTr("or")
				  ).arg(
					  "www.personalausweisportal.de"
				  )
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
		//: LABEL ANDROID IOS
		text: qsTr("You can always access this tutorial again from the \"Help\" section in the menu bar.")
		textStyle: Style.text.tutorial_header
		horizontalAlignment: Text.AlignHCenter
	}
}
