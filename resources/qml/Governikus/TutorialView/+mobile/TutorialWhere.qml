/*
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


TutorialContent {
	id: baseItem

	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

		//: LABEL ANDROID IOS
		text: qsTr("Where can I use the online ID function?")
		textStyle: Style.text.tutorial_header
		font.italic: true
		horizontalAlignment: Text.AlignHCenter
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		source: "qrc:///images/tutorial/where_overview_question.svg"
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

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

		//: LABEL ANDROID IOS
		text: qsTr("On every website of a provider where you see this icon:")
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

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

		//: LABEL ANDROID IOS
		text: qsTr("By the way, you can find many services directly in the AusweisApp2 <b>provider list</b>.")
		textStyle: Style.text.tutorial_content
		horizontalAlignment: Text.AlignHCenter
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	TutorialShowMenuPath {
		width: parent.width

		newSectionImage: "qrc:///images/tutorial/screenshot_providerlist_%1_%2.png".arg(Constants.layout).arg(SettingsModel.language)
		newSectionPointerY: Constants.is_layout_android ? 0.83 : 0.91
		backgroundIcon:"qrc:///images/tutorial/background_icon_where.svg"
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

		//: LABEL ANDROID IOS
		text: qsTr("The <b>integrated self-disclosure</b> is a special service to view the data saved on your ID card.")
		textStyle: Style.text.tutorial_header
		horizontalAlignment: Text.AlignHCenter
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	TutorialShowMenuPath {
		width: parent.width

		newSectionImage: "qrc:///images/tutorial/screenshot_selfauthentication_%1_%2.png".arg(Constants.layout).arg(SettingsModel.language)
		newSectionPointerY: Constants.is_layout_android ? 0.66 : 0.7
		backgroundIcon: "qrc:///images/tutorial/background_icon_where.svg"
	}

	TutorialSpacer {
		width: parent.width

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

		//: LABEL ANDROID IOS
		text: qsTr("And this is how it works")
		color: Style.color.tutorial_where
	}

	GText {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

		//: LABEL ANDROID IOS
		text: qsTr("The AusweisApp2 will always display <b>who</b> wants to access <b>which</b> of your data.")
		textStyle: Style.text.tutorial_header
		horizontalAlignment: Text.AlignHCenter
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		source: "qrc:///images/tutorial/where_userdata_example_%1.svg".arg(SettingsModel.language)
		width: parent.width * 0.8
		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	GText {
		width: parent.width * 0.8
		anchors.horizontalCenter: parent.horizontalCenter

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

		//: LABEL ANDROID IOS
		text: qsTr("To allow the shown service access to the requested data click \"Proceed to PIN entry\"")
		textStyle: Style.text.tutorial_header_secondary
		horizontalAlignment: Text.AlignHCenter
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		source: "qrc:///images/tutorial/where_identify_now_%1.svg".arg(SettingsModel.language)
		width: parent.width
		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	GText {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

		text: (Constants.is_layout_ios ?
			   //: LABEL IOS
			   qsTr("Now lay down your ID card and hold the top of your iPhone to the ID card.") :
			   //: LABEL ANDROID
			   qsTr("Now place your ID card on the NFC-interface of your smartphone.") + " " +
			   //: LABEL ANDROID
			   qsTr("The correct position is specific for your device. If a position does not work try a different one. The AusweisApp2 shows different common positions. If your device is unable to detect your ID card try to check the device capabilities by clicking on \"Check device and ID card\" on the start page.")
			  )
		textStyle: Style.text.tutorial_header_secondary
		horizontalAlignment: Text.AlignHCenter
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		source: "qrc:///images/tutorial/where_lay_down_id.svg"
		width: parent.width * 0.7
		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
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
			rightPadding: Constants.component_spacing

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

			text: (Constants.is_layout_ios ?
				   //: LABEL IOS
				   qsTr("Do not move your iPhone during the procedure!") :
				   //: LABEL ANDROID
				   qsTr("Do not move your device during the procedure!")
				  )
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

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

			text: (SettingsModel.language === "en" ?
				   //: LABEL ANDROID IOS
				   qsTr("Enter") :
				   //: LABEL ANDROID IOS
				   qsTr("Now")
				  )
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter

			x: (parent.width * 0.5) - (width / 2)
			y: (parent.height * 0.05) - (height / 2)
		}

		TutorialImage {
			id: pin6Image
			source: "qrc:///images/tutorial/where_pin6.svg"
			z: 1

			width: parent.width * 0.8
			centerY: 0.5
			centerX: 0.5
		}

		GText {
			width: parent.width

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

			//: LABEL ANDROID IOS
			text: qsTr("six-digit PIN")
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

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

			text: (SettingsModel.language === "en" ?
				   //: LABEL ANDROID IOS
				   qsTr("now!") :
				   //: LABEL ANDROID IOS
				   qsTr("enter!")
				  )
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter

			x: (parent.width * 0.5) - (width / 2)
			y: (parent.height * 0.9) - (height / 2)
		}
	}
}
