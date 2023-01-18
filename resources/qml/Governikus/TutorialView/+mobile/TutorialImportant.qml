/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.PinResetInformationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ChangePinModel 1.0

TutorialContent {
	id: baseItem
	signal letsGoClicked
	signal showPinManagement

	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter
		text: (SettingsModel.language === "en" ?
			//: LABEL ANDROID IOS
			qsTr("Please exchange your") :
			//: LABEL ANDROID IOS
			qsTr("Before you use the online ID function please change the"))
		textStyle: Style.text.tutorial_header
		width: parent.width * 0.9
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}
	Column {
		spacing: Constants.component_spacing
		width: parent.width

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("five-digit")
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.9
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("Transport PIN")
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.9
		}
		TutorialImage {
			source: "qrc:///images/tutorial/important_pin5.svg"
			width: parent.width * 0.8
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}
	Column {
		spacing: Constants.component_spacing
		width: parent.width

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("with a")
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.9
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("six-digit PIN")
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.9
		}
		TutorialImage {
			source: "qrc:///images/tutorial/important_pin6.svg"
			width: parent.width * 0.8
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			text: (SettingsModel.language === "en" ?
				//: LABEL ANDROID IOS
				qsTr("before you use the online ID function!") :
				//: LABEL ANDROID IOS
				qsTr("change!"))
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.9
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("The Transport PIN is sent to you by the Bundesdruckerei via mail.")
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.9
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}
	Column {
		spacing: Constants.component_spacing
		width: parent.width

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("Select for this purpose the menu item \"Change my (Transport) PIN\" from the start page. Later you can also change your six-digit PIN here")
			textStyle: Style.text.tutorial_header_secondary
			width: parent.width * 0.9
		}
		TutorialShowMenuPath {
			backgroundIcon: "qrc:///images/tutorial/background_icon_important.svg"
			newSectionImage: "qrc:///images/tutorial/screenshot_pin_management_menu_%1_%2.png".arg(Constants.layout).arg(SettingsModel.language)
			newSectionPointerY: Constants.is_layout_android ? 0.47 : 0.5
			width: parent.width
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("... or click this button to change your PIN right now:")
			textStyle: Style.text.tutorial_header_secondary
			width: parent.width * 0.9
		}
		GButton {
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.margins: Constants.component_spacing
			//: LABEL ANDROID IOS
			text: qsTr("Change my (Transport) PIN")

			onClicked: showPinManagement()
		}
	}
	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter

		//: LABEL ANDROID IOS
		text: qsTr("Please note: The Transport PIN can only be used for your first PIN change. If you have already set your six-digit PIN (e.g. while picking up your ID card) only the set PIN is valid.")
		textStyle: Style.text.tutorial_header_secondary
		width: parent.width * 0.9
	}
	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter
		text: PinResetInformationModel.pinForgottenTutorialHint
		textStyle: Style.text.tutorial_header_secondary
		width: parent.width * 0.9
	}
	GButton {
		anchors.horizontalCenter: parent.horizontalCenter
		text: PinResetInformationModel.pinResetActionText
		visible: text !== ""

		onClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}
	Column {
		spacing: Constants.component_spacing
		width: parent.width

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
				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/video-passwords".arg(SettingsModel.language))
			}
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("Learn more about this in the YouTube video")
			textStyle: Style.text.tutorial_header_secondary
			width: parent.width * 0.9
		}
		Item {
			height: letsGoImage.height
			width: parent.width

			MouseArea {
				anchors.fill: parent

				onClicked: baseItem.letsGoClicked()
			}
			TutorialImage {
				id: letsGoImage
				centerX: 0.5
				centerY: 0.5
				source: "qrc:///images/tutorial/important_lets_go.svg"
				width: parent.width
			}
			GText {
				color: Constants.white
				horizontalAlignment: Text.AlignHCenter
				//: LABEL ANDROID IOS
				text: qsTr("Let's go")
				textStyle: Style.text.tutorial_header
				width: parent.width
				x: (parent.width * 0.5) - (width / 2)
				y: (parent.height * 0.6) - (height / 2)
			}
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("Do you still have questions?")
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.9
		}
		Image {
			anchors.horizontalCenter: parent.horizontalCenter
			fillMode: Image.PreserveAspectFit
			height: width
			source: "qrc:///images/tutorial/important_space_questionmark.svg"
			width: parent.width * 0.4
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("You can read our <b>FAQs</b> or <b>write</b> to us...")
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.9
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			text: "<a href=\"https://%1\">%1/%2</a><br>%3<br><a href=\"https://%4\">%4</a>".arg("www.ausweisapp.bund.de").arg(SettingsModel.language).arg(
				//: LABEL ANDROID IOS
				qsTr("or")).arg("www.personalausweisportal.de")
			textStyle: Style.text.tutorial_header_secondary
			width: parent.width * 0.9
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_important.svg"
	}
	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ANDROID IOS
		text: qsTr("You can always access this tutorial again from the \"Help\" section in the menu bar.")
		textStyle: Style.text.tutorial_header
		width: parent.width * 0.9
	}
}
