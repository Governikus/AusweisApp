import QtQuick 2.10

import Governikus.Global 1.0

TutorialContent {
	id: baseItem

	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9
		text: qsTr("Where can I use the online ID function?") + settingsModel.translationTrigger
		font.family: "Noto Serif"
		font.pixelSize: Constants.tutorial_content_header_h1_font_size
		font.italic: true
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
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

	Text {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter
		text: qsTr("On every website of a service provider where you see this icon:") + settingsModel.translationTrigger
		font.family: "Noto Serif"
		font.pixelSize: Constants.tutorial_content_header_h2_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.2
		source: "qrc:///images/npa.svg"

		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
	}


	Text {
		width: parent.width * 0.8
		anchors.horizontalCenter: parent.horizontalCenter
		text: qsTr("By the way, you can find many services directly in the AusweisApp2 <b>provider list</b>.") + settingsModel.translationTrigger
		font.family: "Noto Serif"
		font.pixelSize: Constants.tutorial_content_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	Image {
		source: qsTr("qrc:///images/tutorial/generated/where_providerlist_screenshot_en.svg") + settingsModel.translationTrigger
		width: parent.width
		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9
		text: qsTr("The <b>integrated self-disclosure</b> is a special service to view the data saved on your ID card.") + settingsModel.translationTrigger
		font.family: "Noto Serif"
		font.pixelSize: Constants.tutorial_content_header_h1_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_where.svg"
	}

	Text {
		width: parent.width * 0.8
		anchors.horizontalCenter: parent.horizontalCenter
		text: qsTr("You can access it by clicking \"See my personal data\" on the AusweisApp2 start page, followed by \"Identify now\"") + settingsModel.translationTrigger
		font.family: "Noto Serif"
		font.pixelSize: Constants.tutorial_content_header_h2_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		source: qsTr("qrc:///images/tutorial/generated/where_identify_now_en.svg") + settingsModel.translationTrigger
		width: parent.width
		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
	}

	TutorialSpacer {
		width: parent.width
		text: qsTr("And this is how it works") + settingsModel.translationTrigger
		color: Constants.tutorial_green
	}

	Text {
		width: parent.width * 0.9
		anchors.horizontalCenter: parent.horizontalCenter
		text: qsTr("The AusweisApp2 will always display <b>who</b> wants to access <b>which</b> of your data.") + settingsModel.translationTrigger
		font.family: "Noto Serif"
		font.pixelSize: Constants.tutorial_content_header_h1_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		source: qsTr("qrc:///images/tutorial/generated/where_userdata_example_en.svg") + settingsModel.translationTrigger
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

	Text {
		width: parent.width * 0.8
		anchors.horizontalCenter: parent.horizontalCenter
		text: qsTr("Now lay down your ID card and place your device on the ID card.") + settingsModel.translationTrigger
		font.family: "Noto Serif"
		font.pixelSize: Constants.tutorial_content_header_h1_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
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

		Text {
			width: parent.width * 0.6
			text: qsTr("Don't move your device during the procedure!") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_font_size
			horizontalAlignment: Text.AlignLeft
			wrapMode: Text.WordWrap

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

		Text {
			width: parent.width * 0.8
			text: (settingsModel.language === "en" ? qsTr("Enter") : qsTr("Now")) + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap

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

		Text {
			width: parent.width
			text: qsTr("6 digits long PIN") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.bold: true
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
			z: 2

			x: (parent.width * 0.5) - (width / 2)
			y: (parent.height * 0.2) - (height / 2)
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.8
			text: (settingsModel.language === "en" ? qsTr("now!") : qsTr("enter!")) + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap

			x: (parent.width * 0.5) - (width / 2)
			y: (parent.height * 0.9) - (height / 2)
		}
	}
}
