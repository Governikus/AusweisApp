/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: baseItem

	titleBarVisible: false
	automaticSafeAreaMarginHandling: false
	navigationAction: NavigationAction { action: NavigationAction.Action.Back; onClicked: pop() }
	//: LABEL ANDROID IOS
	title: qsTr("Tutorial: Smartphone as card reader")

	signal quitTutorialClicked()

	Rectangle {
		anchors.fill: parent
		color: Constants.white
		z: -1
	}

	content: Item {
		width: baseItem.width
		height: content.contentHeight

		TutorialContent {
			id: content
			visible: true
			width: Constants.is_tablet ? baseItem.width * 0.5 : baseItem.width
			height: content.contentHeight
			anchors.horizontalCenter: parent.horizontalCenter

			GSpacer {
				width: parent.width
				height: statusBar.height
			}

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				Rectangle {
					anchors.horizontalCenter: parent.horizontalCenter
					height: radius * 2
					width: radius * 2
					radius: numberTwo.height
					border.width: 3
					border.color: Style.color.tutorial_how

					GText {
						id: numberThree
						anchors.centerIn: parent

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

						text: "3"
						textStyle: Style.text.tutorial_header_secondary
						font.bold: true
						horizontalAlignment: Text.AlignHCenter
					}
				}

				Item {
					width: parent.width
					height: sacMobileImage.height

					Rectangle {
						anchors.horizontalCenter: parent.horizontalCenter
						height: radius * 2
						width: radius * 2
						radius: parent.width * 0.06
						border.width: 3
						border.color: Style.color.tutorial_how

						y: (parent.height * 0.4) - (height / 2)
						x: (parent.width * 0.5) - (width / 2)

						TutorialImage {
							source: "qrc:///images/tutorial/wifi.svg"
							width: parent.width * 0.8

							centerX: 0.5
							centerY: 0.5
						}
					}

					TutorialImage {
						id: sacMobileImage
						source: "qrc:///images/tutorial/how_method_sac_mobile.svg"
						width: parent.width

						centerX: 0.5
						centerY: 0.5
					}

					GText {
						width: parent.width * 0.4

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

						//: LABEL ANDROID IOS
						text: qsTr("App on tablet or smartphone <b>without</b> NFC chip")
						textStyle: Style.text.tutorial_content
						horizontalAlignment: Text.AlignHCenter

						x: (parent.width * 0.25) - (width / 2)
						y: (parent.height * 0.95) - (height / 2)
					}

					GText {
						width: parent.width * 0.4

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

						//: LABEL ANDROID IOS
						text: qsTr("Smartphone <b>with</b> NFC chip as card reader")
						textStyle: Style.text.tutorial_content
						horizontalAlignment: Text.AlignHCenter

						x: (parent.width * 0.75) - (width / 2)
						y: (parent.height * 0.95) - (height / 2)
					}
				}
			}

			Image {
				source: "qrc:///images/tutorial/arrow_blue.svg"
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.2
				height: width * (sourceSize.height / sourceSize.width)
				fillMode: Image.PreserveAspectFit
				rotation: 90
			}

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("Install AusweisApp2 on both your device without NFC <b>and</b> your smartphone with NFC capability.")
					textStyle: Style.text.tutorial_header
					horizontalAlignment: Text.AlignHCenter
				}

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.8
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
					source: "qrc:///images/tutorial/reader_sac_aa2_ok.svg"
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.3
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
					source: "qrc:///images/tutorial/hint.svg"
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("Both devices have to be connected to the same WiFi network")
					textStyle: Style.text.tutorial_header
					font.bold: true
					horizontalAlignment: Text.AlignHCenter
				}

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.3
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
					source: "qrc:///images/tutorial/wifi.svg"
				}
			}

			Image {
				source: "qrc:///images/tutorial/arrow_blue.svg"
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.2
				height: width * (sourceSize.height / sourceSize.width)
				fillMode: Image.PreserveAspectFit
				rotation: 90
			}

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("Now choose \"Remote\" in the AusweisApp2 on your smartphone...")
					textStyle: Style.text.tutorial_header
					horizontalAlignment: Text.AlignHCenter
				}

				Image {
					source: "qrc:///images/tutorial/reader_sac_menu_%1_%2.svg".arg(Constants.layout).arg(SettingsModel.language)
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				GText {
					anchors.horizontalCenter: parent.horizontalCenter

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("Now")
					textStyle: Style.text.tutorial_header
					horizontalAlignment: Text.AlignRight
				}

				GButton {
					id: pairingButton
					anchors.horizontalCenter: parent.horizontalCenter

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("Start pairing")
					animationsDisabled: true
				}

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					source: "qrc:///images/tutorial/up_icon.svg"
					rotation: 180
					width: parent.width * 0.1
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				Item {
					anchors.horizontalCenter: parent.horizontalCenter
					width: pairingCodeText.width
					height: greyBackgroundRect.height

					Rectangle {
						id: greyBackgroundRect
						anchors.horizontalCenter: parent.horizontalCenter
						width: pairingCodeText.width - 40
						height: width
						color: Style.color.tutorial_box_background
					}

					GText {
						id: pairingCodeText
						anchors.horizontalCenter: parent.horizontalCenter
						anchors.top: greyBackgroundRect.top
						topPadding: 30

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

						//: LABEL ANDROID IOS
						text: qsTr("Pairing code")
						textStyle: Style.text.tutorial_header
						font.bold: true
						horizontalAlignment: Text.AlignHCenter
					}

					GText {
						id: appearingText
						anchors.bottom: greyBackgroundRect.bottom
						bottomPadding: 30
						anchors.horizontalCenter: parent.horizontalCenter

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

						//: LABEL ANDROID IOS
						text: qsTr("appears!")
						textStyle: Style.text.tutorial_header
						horizontalAlignment: Text.AlignHCenter
					}
				}
			}

			TutorialSeperator {
				visible: Constants.is_layout_ios

				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Item {
				visible: Constants.is_layout_ios

				width: parent.width
				height: Math.max(noticeImage5.height, noticeText5.height)

				TutorialImage {
					id: noticeImage5

					width: parent.width * 0.2

					source: "qrc:///images/tutorial/hint.svg"
					centerY: 0.5
					centerX: 0.2
				}

				GText {
					id: noticeText5

					x: (parent.width * 0.65) - (width / 2)
					y: (parent.height * 0.5) - (height / 2)
					width: parent.width * 0.6

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL IOS
					text: qsTr("On the first use of the Smartphone as card reader (SaC), iOS will asks for your permission to access the local network. This permission is required in order find and connect to your SaC. After the first request you can always access the permission in the iOS settings for this app.")
					textStyle: Style.text.tutorial_content
					font.bold: true
					horizontalAlignment: Text.AlignLeft
				}
			}

			Image {
				source: "qrc:///images/tutorial/arrow_blue.svg"
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.2
				height: width * (sourceSize.height / sourceSize.width)
				fillMode: Image.PreserveAspectFit
				rotation: 90
			}

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				Image {
					source: "qrc:///images/tutorial/reader_sac_no_nfc_devices.svg"
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.5
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					text: (Constants.is_layout_ios ?
						   //: LABEL IOS
						   qsTr("Now open the AusweisApp2 on your device <b>without</b> NFC and select <b>Configure remote service</b>.") :
						   //: LABEL ANDROID
						   qsTr("Now open the AusweisApp2 on your device <b>without</b> NFC and select <b>Smartphone as card reader</b>.")
						  )
					textStyle: Style.text.tutorial_header
					horizontalAlignment: Text.AlignHCenter
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("Now select <b>Settings</b>.")
					textStyle: Style.text.tutorial_header
					horizontalAlignment: Text.AlignHCenter
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				Image {
					source: "qrc:///images/tutorial/phone_list.svg"
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.5
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.7

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("Choose smartphone from list")
					textStyle: Style.text.tutorial_header
					horizontalAlignment: Text.AlignHCenter
				}

				Item {
					width: parent.width
					height: noticeImage4.height

					TutorialImage {
						id: noticeImage4
						source: "qrc:///images/tutorial/hint.svg"

						width: parent.width * 0.2
						centerY: 0.5
						centerX: 0.2
					}

					GText {
						width: parent.width * 0.6

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

						//: LABEL ANDROID IOS
						text: qsTr("Enter pairing code next.")
						textStyle: Style.text.tutorial_content
						font.bold: true
						horizontalAlignment: Text.AlignLeft

						x: (parent.width * 0.65) - (width / 2)
						y: (parent.height * 0.5) - (height / 2)
					}
				}
			}

			Image {
				source: "qrc:///images/tutorial/arrow_blue.svg"
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.2
				height: width * (sourceSize.height / sourceSize.width)
				fillMode: Image.PreserveAspectFit
				rotation: 90
			}

			Column {
				width: parent.width

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					source: "qrc:///images/tutorial/reader_sac_no_nfc_provider.svg"
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
					width: parent.width * 0.5
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.6

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("Click link on the website of the provider on the device <b>without</b> NFC.")
					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_header_secondary
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Item {
				width: parent.width
				height: userdataExample.height + textAccessWhoWhat.height + textOpenAutomatic.height + 2 * Constants.component_spacing

				TutorialImage {
					centerX: 0.2
					centerY: (textAccessWhoWhat.y + textAccessWhoWhat.height) / (2 * parent.height)
					width: parent.width * 0.15

					source: "qrc:///images/tutorial/tablet-no-nfc.svg"
				}

				GText {
					id: textOpenAutomatic

					width: parent.width * 0.6
					anchors {
						top: parent.top
						right: parent.right
					}

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("The App opens automatically.")
					rightPadding: Constants.component_spacing
					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_content
				}

				GText {
					id: textAccessWhoWhat

					width: parent.width * 0.6
					anchors {
						top: textOpenAutomatic.bottom
						topMargin: Constants.component_spacing
						right: parent.right
					}

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("The AusweisApp2 will display who wants to access which data.")
					rightPadding: Constants.component_spacing
					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_header_secondary
				}

				TutorialImage {
					id: userdataExample

					width: parent.width * 0.8
					anchors {
						top: textAccessWhoWhat.bottom
						topMargin: Constants.component_spacing
						horizontalCenter: parent.horizontalCenter
					}

					source: "qrc:///images/tutorial/reader_nfc_userdata_example_%1.svg".arg(SettingsModel.language)
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				GText {
					id: startProcessText
					anchors.horizontalCenter: parent.horizontalCenter

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					textStyle: Style.text.tutorial_header_secondary
					//: LABEL ANDROID IOS
					text: qsTr("Start the process with a click on:")
				}

				Row {
					anchors.horizontalCenter: parent.horizontalCenter
					height: authArrow.height
					spacing: Constants.component_spacing

					Image {
						id: authArrow
						source: "qrc:///images/tutorial/arrow_blue.svg"
						width: parent.width * 0.2
						height: width * (sourceSize.height / sourceSize.width)
						fillMode: Image.PreserveAspectFit
					}

					GButton {
						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

						//: LABEL ANDROID IOS
						text: qsTr("Proceed to PIN entry");
						icon.source: "qrc:///images/identify.svg"
						tintIcon: true
						animationsDisabled: true
					}
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				Item {
					width: parent.width
					height: screenshotAuth.height

					TutorialImage {
						id: screenshotAuth

						source: "qrc:///images/tutorial/screenshot_choose_reader_%1_%2.png".arg(Constants.layout).arg(SettingsModel.language)
						width: parent.width * 0.5

						centerX: 0.5
						centerY: 0.4
					}

					TutorialImage {
						id: pointerImage
						source: "qrc:///images/tutorial/hand.svg"
						width: parent.width * 0.1
						centerX: 0.52
						centerY: 0.89
					}
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("Tap on WiFi")
					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_header
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					source: "qrc:///images/tutorial/where_lay_down_id.svg"
					width: parent.width * 0.8
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("... and place the ID card onto the NFC interface.")

					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_header_secondary
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
						width: parent.width * 0.6

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

						//: LABEL ANDROID IOS
						text: qsTr("Do not move device or ID card!")
						textStyle: Style.text.tutorial_content
						font.bold: true
						horizontalAlignment: Text.AlignLeft

						x: (parent.width * 0.65) - (width / 2)
						y: (parent.height * 0.5) - (height / 2)
					}
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Item {
				width: parent.width * 0.9
				height: Math.max(nfcPosition.height, nfcText.height)

				TutorialImage {
					id: nfcPosition

					width: parent.width * 0.75
					anchors {
						left: parent.left
						verticalCenter: parent.verticalCenter
					}

					source: "qrc:///images/tutorial/reader_nfc_smartphone_nfc_position.svg"
				}

				GText {
					id: nfcText

					width: parent.width * 0.55
					anchors {
						right: parent.right
						verticalCenter: parent.verticalCenter
					}

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("The correct position is specific for your device. If a position does not work try a different one. The AusweisApp2 shows different common positions.")
					textStyle: Style.text.tutorial_content
					font.bold: true
					horizontalAlignment: Text.AlignLeft
				}
			}

			GText {
				width: parent.width * 0.9
				anchors.horizontalCenter: parent.horizontalCenter

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

				//: LABEL ANDROID IOS
				text: qsTr("If your device is unable to detect your ID card try to check the device capabilities by clicking on \"Check device and ID card\" on the start page.")
				textStyle: Style.text.tutorial_content
				font.bold: true
				horizontalAlignment: Text.AlignHCenter
			}

			GText {
				width: parent.width * 0.9
				visible: Constants.is_layout_android
				anchors.horizontalCenter: parent.horizontalCenter

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

				//: LABEL ANDROID IOS
				text: qsTr("You can find more information on compatible devices on our %1mobile device list%2.").arg(
							"<a href=\"https://www.ausweisapp.bund.de/%1/aa2/mobile-devices\">".arg(SettingsModel.language)
						).arg("</a>")
				horizontalAlignment: Text.AlignHCenter
				textStyle: Style.text.tutorial_header_secondary
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				Item {
					width: parent.width
					height: pin6Image.height + Constants.component_spacing * 2

					GText {
						width: parent.width * 0.8

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

						//: LABEL ANDROID IOS This is the first of three lines "Enter" "six-digit PIN" "now!" and should be translated accoording to the third line.
						text: qsTr("Enter")
						textStyle: Style.text.tutorial_header
						horizontalAlignment: Text.AlignHCenter

						x: (parent.width * 0.5) - (width / 2)
						y: (parent.height * 0.05) - (height / 2)
					}

					TutorialImage {
						id: pin6Image
						source: "qrc:///images/tutorial/reader_nfc_pin6.svg"
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

						//: LABEL ANDROID IOS This is the third of three lines "Enter" "six-digit PIN" "now!" and should be translated accoording to the first line.
						text: qsTr("now!")
						textStyle: Style.text.tutorial_header
						horizontalAlignment: Text.AlignHCenter

						x: (parent.width * 0.5) - (width / 2)
						y: (parent.height * 0.9) - (height / 2)
					}
				}

				Item {
					width: parent.width
					height: Math.max(noticeImage2.height, noticeText2.height)

					TutorialImage {
						id: noticeImage2
						source: "qrc:///images/tutorial/hint.svg"

						width: parent.width * 0.2
						centerY: 0.5
						centerX: 0.2
					}

					GText {
						id: noticeText2
						width: parent.width * 0.6

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

						//: LABEL ANDROID IOS
						text: qsTr("This is only possible if you have exchanged the five-digit Transport PIN with a six-digit PIN beforehand.")
						textStyle: Style.text.tutorial_content
						font.bold: true
						horizontalAlignment: Text.AlignLeft

						x: (parent.width * 0.65) - (width / 2)
						y: (parent.height * 0.5) - (height / 2)
					}
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			MouseArea {
				width: parent.width
				height: finishedButton.height

				Image {
					id: finishedButton
					source: "qrc:///images/tutorial/reader_nfc_finished.svg"
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.8
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				onClicked: pop()
			}

			GSpacer {
				width: parent.width
				height: footer.height
			}
		}
	}

	TutorialStatusBar {
		id: statusBar
	}

	TutorialReaderMethodFooter {
		id: footer
		width: baseItem.width

		onMenuClicked: pop()
		onQuitTutorialClicked: {
			pop()
			baseItem.quitTutorialClicked()
		}
	}
}
