/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


TutorialContent {
	id: baseItem

	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9
		//: LABEL ANDROID IOS
		text: qsTr("Where can I use the online ID function?") + SettingsModel.translationTrigger
		textStyle: Style.text.tutorial_header
		font.italic: true
		horizontalAlignment: Text.AlignHCenter
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		source: "qrc:///images/tutorial/generated/where_overview_question.svg"
		width: parent.width * 0.75
		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	GText {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter
		//: LABEL ANDROID IOS
		text: qsTr("On every website of a service provider where you see this icon:") + SettingsModel.translationTrigger
		textStyle: Style.text.tutorial_header_secondary
		horizontalAlignment: Text.AlignHCenter
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.2
		source: "qrc:///images/npa.svg"

		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
	}


	GText {
		width: parent.width * 0.8
		anchors.horizontalCenter: parent.horizontalCenter
		//: LABEL ANDROID IOS
		text: qsTr("By the way, you can find many services directly in the AusweisApp2 <b>provider list</b>.") + SettingsModel.translationTrigger
		textStyle: Style.text.tutorial_content
		horizontalAlignment: Text.AlignHCenter
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	Image {
		//: LABEL ANDROID
		source: qsTr("qrc:///images/tutorial/generated/where_providerlist_screenshot_android_en.svg") + SettingsModel.translationTrigger
		width: parent.width
		height: width * (sourceSize.height / sourceSize.width)
		visible: Constants.is_layout_android
		fillMode: Image.PreserveAspectFit
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		//: LABEL IOS
		source: qsTr("qrc:///images/tutorial/screenshot_providerlist_ios_en.png") + SettingsModel.translationTrigger
		width: parent.width * 0.6
		height: width * (sourceSize.height / sourceSize.width)
		visible: Constants.is_layout_ios
		fillMode: Image.PreserveAspectFit
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9
		//: LABEL ANDROID IOS
		text: qsTr("The <b>integrated self-disclosure</b> is a special service to view the data saved on your ID card.") + SettingsModel.translationTrigger
		textStyle: Style.text.tutorial_header
		horizontalAlignment: Text.AlignHCenter
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	GText {
		width: parent.width * 0.8
		anchors.horizontalCenter: parent.horizontalCenter
		//: LABEL ANDROID IOS
		text: qsTr("You can access the self-disclosure by clicking \"See my personal data\" on the AusweisApp2 start page, followed by \"Proceed to PIN entry\"") + SettingsModel.translationTrigger
		textStyle: Style.text.tutorial_header_secondary
		horizontalAlignment: Text.AlignHCenter
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		//: LABEL ANDROID IOS
		source: qsTr("qrc:///images/tutorial/generated/where_identify_now_en.svg") + SettingsModel.translationTrigger
		width: parent.width
		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
	}

	TutorialSpacer {
		width: parent.width
		//: LABEL ANDROID IOS
		text: qsTr("And this is how it works") + SettingsModel.translationTrigger
		color: Style.color.tutorial_where
	}

	GText {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter
		//: LABEL ANDROID IOS
		text: qsTr("The AusweisApp2 will always display <b>who</b> wants to access <b>which</b> of your data.") + SettingsModel.translationTrigger
		textStyle: Style.text.tutorial_header
		horizontalAlignment: Text.AlignHCenter
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		//: LABEL ANDROID IOS
		source: qsTr("qrc:///images/tutorial/generated/where_userdata_example_en.svg") + SettingsModel.translationTrigger
		width: parent.width * 0.8
		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		source: "qrc:///images/tutorial/generated/where_lay_down_id.svg"
		width: parent.width * 0.7
		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
	}

	GText {
		width: parent.width * 0.8
		anchors.horizontalCenter: parent.horizontalCenter
		//: LABEL ANDROID IOS
		text: (Constants.is_layout_ios ? qsTr("Now lay down your ID card and hold the top of your iPhone to the ID card.")
									 : qsTr("Now lay down your ID card and place your device on the ID card."))
									 + SettingsModel.translationTrigger
		textStyle: Style.text.tutorial_header
		horizontalAlignment: Text.AlignHCenter
	}

	Item {
		width: parent.width
		height: noticeImage.height

		TutorialImage {
			id: noticeImage
			source: "qrc:///images/tutorial/hint.svg"

			width: parent.width * 0.2
			centerY: 0.5
			centerX: 0.2
		}

		GText {
			width: parent.width * 0.5
		//: LABEL ANDROID IOS
			text: (Constants.is_layout_ios ? qsTr("Don't move your iPhone during the procedure!")
										 : qsTr("Don't move your device during the procedure!"))
										 + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_content
			horizontalAlignment: Text.AlignLeft

			x: (parent.width * 0.75) - (width / 2)
			y: (parent.height * 0.5) - (height / 2)
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	Item {
		width: parent.width
		height: pin6Image.height + Constants.component_spacing * 2

		GText {
			width: parent.width * 0.8
			//: LABEL ANDROID IOS
			text: (SettingsModel.language === "en" ? qsTr("Enter") : qsTr("Now")) + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter

			x: (parent.width * 0.5) - (width / 2)
			y: (parent.height * 0.05) - (height / 2)
		}

		TutorialImage {
			id: pin6Image
			source: "qrc:///images/tutorial/generated/where_pin6.svg"
			z: 1

			width: parent.width * 0.8
			centerY: 0.5
			centerX: 0.5
		}

		GText {
			width: parent.width
			//: LABEL ANDROID IOS
			text: qsTr("6 digits long PIN") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			font.bold: true
			horizontalAlignment: Text.AlignHCenter
			z: 2

			x: (parent.width * 0.5) - (width / 2)
			y: (parent.height * 0.2) - (height / 2)
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.8
			//: LABEL ANDROID IOS
			text: (SettingsModel.language === "en" ? qsTr("now!") : qsTr("enter!")) + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter

			x: (parent.width * 0.5) - (width / 2)
			y: (parent.height * 0.9) - (height / 2)
		}
	}
}
