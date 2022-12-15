/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

TutorialContent {
	id: baseItem
	signal firePush(var pSectionPage)
	signal quitTutorialClicked

	Item {
		Component {
			id: readerMethodNfc
			TutorialReaderMethodNfc {
				onQuitTutorialClicked: baseItem.quitTutorialClicked()
			}
		}
		Component {
			id: readerMethodSacMobile
			TutorialReaderMethodSacMobile {
				onQuitTutorialClicked: baseItem.quitTutorialClicked()
			}
		}
		Component {
			id: readerMethodSacDesktop
			TutorialReaderMethodSacDesktop {
				onQuitTutorialClicked: baseItem.quitTutorialClicked()
			}
		}
	}
	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		font.italic: true
		horizontalAlignment: Text.AlignHCenter

		//: INFO ANDROID IOS
		text: (Constants.is_layout_ios ? qsTr("How can I use the AusweisApp2 on my iPhone?") : qsTr("How can I use the AusweisApp2 on my smartphone?"))
		textStyle: Style.text.tutorial_header
		width: parent.width * 0.9

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}
	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		fillMode: Image.PreserveAspectFit
		height: width * (sourceSize.height / sourceSize.width)
		source: "qrc:///images/tutorial/how_questions_everywhere.svg"
		width: parent.width * 0.9
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}
	Column {
		spacing: Constants.component_spacing
		width: parent.width

		Item {
			height: Math.max(noticeText.height, noticeImage.height)
			width: parent.width

			TutorialImage {
				id: noticeImage
				centerX: 0.2
				centerY: 0.5
				source: "qrc:///images/tutorial/hint.svg"
				width: parent.width * 0.2
			}
			GText {
				id: noticeText
				font.bold: true
				horizontalAlignment: Text.AlignLeft

				//: INFO ANDROID IOS
				text: (Constants.is_layout_ios ? qsTr("Many iPhones (iPhone 7 and newer) can access the ID card via the built-in NFC interface.") : qsTr("Many Android devices can access the ID card via the built-in NFC interface."))
				textStyle: Style.text.tutorial_content
				width: parent.width * 0.6
				x: (parent.width * 0.65) - (width / 2)
				y: (parent.height * 0.5) - (height / 2)

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
			}
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("You can test the capabilities of your device and your card by choosing \"Check device and ID card\" on the start page:")
			textStyle: Style.text.tutorial_header_secondary
			width: parent.width * 0.9
		}
		TutorialShowMenuPath {
			backgroundIcon: "qrc:///images/tutorial/background_icon_how.svg"
			newSectionImage: "qrc:///images/tutorial/screenshot_check_id_card_%1_%2.png".arg(Constants.layout).arg(SettingsModel.language)
			newSectionPointerY: Constants.is_layout_android ? 0.3 : 0.29
			width: parent.width
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("You can also find a list of compatible NFC-capable smartphones here:")
			textStyle: Style.text.tutorial_header_secondary
			width: parent.width * 0.9
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			text: "<a href=\"%1\">%1</a>".arg("https://www.ausweisapp.bund.de/%1/aa2/mobile-devices".arg(SettingsModel.language))
			textStyle: Style.text.tutorial_content
			width: parent.width * 0.9
		}
		Image {
			anchors.horizontalCenter: parent.horizontalCenter
			fillMode: Image.PreserveAspectFit
			height: width * (sourceSize.height / sourceSize.width)
			source: "qrc:///images/tutorial/how_device_lineup.svg"
			width: parent.width
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			font.italic: true
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("The AusweisApp2 offers the following options to access your ID card:")
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.9

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}
	Rectangle {
		anchors.horizontalCenter: parent.horizontalCenter
		border.color: Style.color.tutorial_how
		border.width: 3
		color: Constants.white
		height: methodNfcSection.height
		width: parent.width * 0.95

		MouseArea {

			//: LABEL ANDROID IOS
			Accessible.name: qsTr("Direct connection via NFC chip tutorial")
			Accessible.role: Accessible.Button
			anchors.fill: parent

			Accessible.onPressAction: firePush(readerMethodNfc)
			onClicked: firePush(readerMethodNfc)
		}
		Column {
			id: methodNfcSection
			padding: 10
			spacing: Constants.component_spacing
			width: parent.width

			GText {
				anchors.horizontalCenter: parent.horizontalCenter
				font.bold: true
				horizontalAlignment: Text.AlignHCenter

				//: LABEL ANDROID IOS
				text: qsTr("Direct connection via NFC chip")
				textStyle: Style.text.tutorial_header_secondary
				width: parent.width * 0.9

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
			}
			Rectangle {
				anchors.horizontalCenter: parent.horizontalCenter
				border.color: Style.color.tutorial_how
				border.width: 3
				height: radius * 2
				radius: numberOne.height
				width: radius * 2

				GText {
					id: numberOne
					anchors.centerIn: parent
					font.bold: true
					horizontalAlignment: Text.AlignHCenter
					text: "1"
					textStyle: Style.text.tutorial_header_secondary

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
			}
			Image {
				anchors.horizontalCenter: parent.horizontalCenter
				fillMode: Image.PreserveAspectFit
				height: width * (sourceSize.height / sourceSize.width)
				source: "qrc:///images/tutorial/how_method_nfc.svg"
				width: parent.width * 0.8
			}
			GText {
				anchors.horizontalCenter: parent.horizontalCenter
				horizontalAlignment: Text.AlignHCenter

				//: LABEL ANDROID IOS
				text: (Constants.is_layout_ios ? qsTr("App on iPhone <b>with</b> NFC chip as card reader") : qsTr("App on Android smartphone <b>with</b> NFC chip as card reader"))
				textStyle: Style.text.tutorial_content
				width: parent.width * 0.6

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
			}
			Image {
				anchors.horizontalCenter: parent.horizontalCenter
				fillMode: Image.PreserveAspectFit
				height: width * (sourceSize.height / sourceSize.width)
				source: "qrc:///images/tutorial/arrow_blue.svg"
				width: parent.width * 0.2
			}
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}
	Rectangle {
		anchors.horizontalCenter: parent.horizontalCenter
		border.color: Style.color.tutorial_how
		border.width: 3
		color: Constants.white
		height: methodSacDesktopSection.height
		width: parent.width * 0.95

		MouseArea {

			//: LABEL ANDROID IOS
			Accessible.name: qsTr("Smartphone as card reader tutorial")
			Accessible.role: Accessible.Button
			anchors.fill: parent

			Accessible.onPressAction: firePush(readerMethodSacDesktop)
			onClicked: firePush(readerMethodSacDesktop)
		}
		Column {
			id: methodSacDesktopSection
			padding: 10
			spacing: Constants.component_spacing
			width: parent.width

			GText {
				anchors.horizontalCenter: parent.horizontalCenter
				font.bold: true
				horizontalAlignment: Text.AlignHCenter

				//: LABEL ANDROID IOS
				text: qsTr("Smartphone as card reader")
				textStyle: Style.text.tutorial_header_secondary
				width: parent.width * 0.9

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
			}
			Rectangle {
				anchors.horizontalCenter: parent.horizontalCenter
				border.color: Style.color.tutorial_how
				border.width: 3
				height: radius * 2
				radius: numberTwo.height
				width: radius * 2

				GText {
					id: numberTwo
					anchors.centerIn: parent
					font.bold: true
					horizontalAlignment: Text.AlignHCenter
					text: "2"
					textStyle: Style.text.tutorial_header_secondary

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
			}
			Item {
				height: sacStationaryImage.height
				width: parent.width

				Rectangle {
					anchors.horizontalCenter: parent.horizontalCenter
					border.color: Style.color.tutorial_how
					border.width: 3
					height: radius * 2
					radius: parent.width * 0.06
					width: radius * 2
					x: (parent.width * 0.5) - (width / 2)
					y: (parent.height * 0.4) - (height / 2)

					TutorialImage {
						centerX: 0.5
						centerY: 0.5
						source: "qrc:///images/tutorial/wifi.svg"
						width: parent.width * 0.8
					}
				}
				TutorialImage {
					id: sacStationaryImage
					centerX: 0.5
					centerY: 0.5
					source: "qrc:///images/tutorial/how_method_sac_desktop.svg"
					width: parent.width
				}
				GText {
					horizontalAlignment: Text.AlignHCenter

					//: LABEL ANDROID IOS
					text: qsTr("App on computer <b>without</b> NFC chip")
					textStyle: Style.text.tutorial_content
					width: parent.width * 0.4
					x: (parent.width * 0.25) - (width / 2)
					y: (parent.height * 0.95) - (height / 2)

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
				GText {
					horizontalAlignment: Text.AlignHCenter

					//: LABEL ANDROID IOS
					text: qsTr("Smartphone <b>with</b> NFC chip as card reader")
					textStyle: Style.text.tutorial_content
					width: parent.width * 0.4
					x: (parent.width * 0.75) - (width / 2)
					y: (parent.height * 0.95) - (height / 2)

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
			}
			Image {
				anchors.horizontalCenter: parent.horizontalCenter
				fillMode: Image.PreserveAspectFit
				height: width * (sourceSize.height / sourceSize.width)
				source: "qrc:///images/tutorial/arrow_blue.svg"
				width: parent.width * 0.2
			}
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}
	Rectangle {
		anchors.horizontalCenter: parent.horizontalCenter
		border.color: Style.color.tutorial_how
		border.width: 3
		color: Constants.white
		height: methodSacMobileSection.height
		width: parent.width * 0.95

		MouseArea {

			//: LABEL ANDROID IOS
			Accessible.name: qsTr("Smartphone as card reader mobile tutorial")
			Accessible.role: Accessible.Button
			anchors.fill: parent

			Accessible.onPressAction: firePush(readerMethodSacMobile)
			onClicked: firePush(readerMethodSacMobile)
		}
		Column {
			id: methodSacMobileSection
			padding: 10
			spacing: Constants.component_spacing
			width: parent.width

			Rectangle {
				anchors.horizontalCenter: parent.horizontalCenter
				border.color: Style.color.tutorial_how
				border.width: 3
				height: radius * 2
				radius: numberTwo.height
				width: radius * 2

				GText {
					id: numberThree
					anchors.centerIn: parent
					font.bold: true
					horizontalAlignment: Text.AlignHCenter
					text: "3"
					textStyle: Style.text.tutorial_header_secondary

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
			}
			Item {
				height: sacMobileImage.height
				width: parent.width

				Rectangle {
					anchors.horizontalCenter: parent.horizontalCenter
					border.color: Style.color.tutorial_how
					border.width: 3
					height: radius * 2
					radius: parent.width * 0.06
					width: radius * 2
					x: (parent.width * 0.5) - (width / 2)
					y: (parent.height * 0.4) - (height / 2)

					TutorialImage {
						centerX: 0.5
						centerY: 0.5
						source: "qrc:///images/tutorial/wifi.svg"
						width: parent.width * 0.8
					}
				}
				TutorialImage {
					id: sacMobileImage
					centerX: 0.5
					centerY: 0.5
					source: "qrc:///images/tutorial/how_method_sac_mobile.svg"
					width: parent.width
				}
				GText {
					horizontalAlignment: Text.AlignHCenter

					//: LABEL ANDROID IOS
					text: qsTr("App on tablet or smartphone <b>without</b> NFC chip")
					textStyle: Style.text.tutorial_content
					width: parent.width * 0.4
					x: (parent.width * 0.25) - (width / 2)
					y: (parent.height * 0.95) - (height / 2)

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
				GText {
					horizontalAlignment: Text.AlignHCenter

					//: LABEL ANDROID IOS
					text: qsTr("Smartphone <b>with</b> NFC chip as card reader")
					textStyle: Style.text.tutorial_content
					width: parent.width * 0.4
					x: (parent.width * 0.75) - (width / 2)
					y: (parent.height * 0.95) - (height / 2)

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
			}
			Image {
				anchors.horizontalCenter: parent.horizontalCenter
				fillMode: Image.PreserveAspectFit
				height: width * (sourceSize.height / sourceSize.width)
				source: "qrc:///images/tutorial/arrow_blue.svg"
				width: parent.width * 0.2
			}
		}
	}
	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}
	Rectangle {
		property alias text: textContent.text

		color: Style.color.tutorial_how
		height: textContent.height + 2 * Constants.component_spacing
		width: parent.width

		GText {
			id: textContent
			anchors.centerIn: parent
			color: Constants.white
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("Another tip")
			textStyle: Style.text.tutorial_header
			width: parent.width * 0.8

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
	}
	Column {
		spacing: Constants.component_spacing
		width: parent.width

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("For lengthy forms, e.g. a BAf\u00F6G application, we recommend you to use the AusweisApp2 on a computer...")
			textStyle: Style.text.tutorial_header_secondary
			width: parent.width * 0.9

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
		Item {
			height: formImage.height
			width: parent.width

			TutorialImage {
				id: formImage
				centerX: 0.4
				centerY: 0.5
				source: "qrc:///images/tutorial/how_form_no_fun.svg"
				width: parent.width * 0.6
			}
			GText {
				color: Style.color.accent
				horizontalAlignment: Text.AlignLeft
				rightPadding: Constants.component_spacing

				//: LABEL ANDROID IOS
				text: qsTr("Filling long forms is no fun on a smartphone!")
				textStyle: Style.text.tutorial_content
				width: parent.width * 0.5
				x: parent.width * 0.5
				y: (parent.height * 0.5) - (height / 2)

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
			}
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter

			//: LABEL ANDROID IOS
			text: qsTr("... and to use a smartphone to communicate with your ID card. A USB reader is of course also an alternative.")
			textStyle: Style.text.tutorial_header_secondary
			width: parent.width * 0.9

			Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
			Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
		}
	}
	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		fillMode: Image.PreserveAspectFit
		height: width * (sourceSize.height / sourceSize.width)
		source: "qrc:///images/tutorial/how_desktop.svg"
		width: parent.width
	}
}
