/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

TutorialContent {
	id: baseItem
	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		font.italic: true
		horizontalAlignment: Text.AlignHCenter

		//: LABEL ANDROID IOS
		text: qsTr("Where can I use the online ID function?")
		textStyle: Style.text.tutorial_header
		width: parent.width * 0.9

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}
	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		fillMode: Image.PreserveAspectFit
		height: width * (sourceSize.height / sourceSize.width)
		source: "qrc:///images/tutorial/where_overview_question.svg"
		width: parent.width * 0.75
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}
	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter

		//: LABEL ANDROID IOS
		text: qsTr("On every website of a provider where you see this icon:")
		textStyle: Style.text.tutorial_header_secondary
		width: parent.width * 0.9

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
	}
	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		fillMode: Image.PreserveAspectFit
		height: width * (sourceSize.height / sourceSize.width)
		source: "qrc:///images/npa.svg"
		width: parent.width * 0.2
	}
	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter

		//: LABEL ANDROID IOS
		text: qsTr("By the way, you can find many services directly in the AusweisApp2 <b>provider list</b>.")
		textStyle: Style.text.tutorial_content
		width: parent.width * 0.8

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}
	TutorialShowMenuPath {
		backgroundIcon: "qrc:///images/tutorial/background_icon_where.svg"
		newSectionImage: "qrc:///images/tutorial/screenshot_providerlist_%1_%2.png".arg(Constants.layout).arg(SettingsModel.language)
		newSectionPointerY: Constants.is_layout_android ? 0.85 : 0.87
		width: parent.width
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}
	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter

		//: LABEL ANDROID IOS
		text: qsTr("The <b>integrated self-authentication</b> is a special service to view the data saved on your ID card.")
		textStyle: Style.text.tutorial_header
		width: parent.width * 0.9

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}
	TutorialShowMenuPath {
		backgroundIcon: "qrc:///images/tutorial/background_icon_where.svg"
		newSectionImage: "qrc:///images/tutorial/screenshot_selfauthentication_%1_%2.png".arg(Constants.layout).arg(SettingsModel.language)
		newSectionPointerY: Constants.is_layout_android ? 0.66 : 0.67
		width: parent.width
	}
	TutorialSpacer {
		color: Style.color.tutorial_where

		//: LABEL ANDROID IOS
		text: qsTr("And this is how it works")
		width: parent.width

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
	}
	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter

		//: LABEL ANDROID IOS
		text: qsTr("The AusweisApp2 will always display <b>who</b> wants to access <b>which</b> of your data.")
		textStyle: Style.text.tutorial_header
		width: parent.width * 0.9

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
	}
	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		fillMode: Image.PreserveAspectFit
		height: width * (sourceSize.height / sourceSize.width)
		source: "qrc:///images/tutorial/where_userdata_example_%1.svg".arg(SettingsModel.language)
		width: parent.width * 0.8
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}
	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter

		//: LABEL ANDROID IOS
		text: qsTr("To allow the shown service access to the requested data click \"Proceed to PIN entry\"")
		textStyle: Style.text.tutorial_header_secondary
		width: parent.width * 0.8

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
	}
	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		fillMode: Image.PreserveAspectFit
		height: width * (sourceSize.height / sourceSize.width)
		source: "qrc:///images/tutorial/where_identify_now_%1.svg".arg(SettingsModel.language)
		width: parent.width
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}
	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		horizontalAlignment: Text.AlignHCenter
		text: (Constants.is_layout_ios ?
			//: LABEL IOS
			qsTr("Now lay down your ID card and hold the top of your iPhone to the ID card.") :
			//: LABEL ANDROID
			qsTr("Now place your ID card on the NFC-interface of your smartphone.") + " " +
			//: LABEL ANDROID
			qsTr("The correct position is specific for your device. If a position does not work try a different one. The AusweisApp2 shows different common positions. If your device is unable to detect your ID card try to check the device capabilities by clicking on \"Check device and ID card\" on the start page."))
		textStyle: Style.text.tutorial_header_secondary
		width: parent.width * 0.9

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
	}
	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		fillMode: Image.PreserveAspectFit
		height: width * (sourceSize.height / sourceSize.width)
		source: "qrc:///images/tutorial/where_lay_down_id.svg"
		width: parent.width * 0.7
	}
	Item {
		height: noticeImage.height
		width: parent.width

		TutorialImage {
			id: noticeImage
			centerX: 0.2
			centerY: 0.5
			source: "qrc:///images/tutorial/hint.svg"
			width: parent.width * 0.2
		}
		GText {
			horizontalAlignment: Text.AlignLeft
			rightPadding: Constants.component_spacing
			text: (Constants.is_layout_ios ?
				//: LABEL IOS
				qsTr("Do not move your iPhone during the procedure!") :
				//: LABEL ANDROID
				qsTr("Do not move your device during the procedure!"))
			textStyle: Style.text.tutorial_content
			width: parent.width * 0.5
			x: (parent.width * 0.75) - (width / 2)
			y: (parent.height * 0.5) - (height / 2)

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}
	Item {
		height: pin6Image.height + Constants.component_spacing * 2
		width: parent.width

		GText {
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS This is the first of three lines "Enter" "six-digit PIN" "now!" and should be translated according to the third line.
			text: qsTr("Enter")
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.8
			x: (parent.width * 0.5) - (width / 2)
			y: (parent.height * 0.05) - (height / 2)

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
		TutorialImage {
			id: pin6Image
			centerX: 0.5
			centerY: 0.5
			source: "qrc:///images/tutorial/where_pin6.svg"
			width: parent.width * 0.8
			z: 1
		}
		GText {
			font.bold: true
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("six-digit PIN")
			textStyle: Style.text.tutorial_header
			width: parent.width
			x: (parent.width * 0.5) - (width / 2)
			y: (parent.height * 0.2) - (height / 2)
			z: 2

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS This is the third of three lines "Enter" "six-digit PIN" "now!" and should be translated according to the first line.
			text: qsTr("now!")
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.8
			x: (parent.width * 0.5) - (width / 2)
			y: (parent.height * 0.9) - (height / 2)

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
	}
}
