/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0

SectionPage {
	id: baseItem
	signal quitTutorialClicked

	automaticSafeAreaMarginHandling: false
	//: LABEL ANDROID IOS
	title: qsTr("Tutorial: Smartphone as card reader")
	titleBarVisible: false

	content: Item {
		height: content.contentHeight
		width: baseItem.width

		TutorialContent {
			id: content
			anchors.horizontalCenter: parent.horizontalCenter
			height: content.contentHeight
			visible: true
			width: Constants.is_tablet ? baseItem.width * 0.5 : baseItem.width

			GSpacer {
				height: statusBar.height
				width: parent.width
			}
			Column {
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
						horizontalAlignment: Text.AlignHCenter
						text: "3"
						textStyle: Style.text.tutorial_header_secondary_highlight

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
			}
			Image {
				anchors.horizontalCenter: parent.horizontalCenter
				fillMode: Image.PreserveAspectFit
				height: width * (sourceSize.height / sourceSize.width)
				rotation: 90
				source: "qrc:///images/tutorial/arrow_blue.svg"
				width: parent.width * 0.2
			}
			Column {
				spacing: Constants.component_spacing
				width: parent.width

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					horizontalAlignment: Text.AlignHCenter

					//: LABEL ANDROID IOS
					text: qsTr("Install AusweisApp2 on both your device without NFC <b>and</b> your smartphone with NFC capability.")
					textStyle: Style.text.tutorial_header
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					fillMode: Image.PreserveAspectFit
					height: width * (sourceSize.height / sourceSize.width)
					source: "qrc:///images/tutorial/reader_sac_aa2_ok.svg"
					width: parent.width * 0.8
				}
			}
			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}
			Column {
				spacing: Constants.component_spacing
				width: parent.width

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					fillMode: Image.PreserveAspectFit
					height: width * (sourceSize.height / sourceSize.width)
					source: "qrc:///images/tutorial/hint.svg"
					width: parent.width * 0.3
				}
				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					horizontalAlignment: Text.AlignHCenter

					//: LABEL ANDROID IOS
					text: qsTr("Both devices have to be connected to the same WiFi network")
					textStyle: Style.text.tutorial_header_highlight
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					fillMode: Image.PreserveAspectFit
					height: width * (sourceSize.height / sourceSize.width)
					source: "qrc:///images/tutorial/wifi.svg"
					width: parent.width * 0.3
				}
			}
			Image {
				anchors.horizontalCenter: parent.horizontalCenter
				fillMode: Image.PreserveAspectFit
				height: width * (sourceSize.height / sourceSize.width)
				rotation: 90
				source: "qrc:///images/tutorial/arrow_blue.svg"
				width: parent.width * 0.2
			}
			Column {
				spacing: Constants.component_spacing
				width: parent.width

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					horizontalAlignment: Text.AlignHCenter

					//: LABEL ANDROID IOS
					text: qsTr("Now choose \"Card reader\" in the AusweisApp2 on your smartphone...")
					textStyle: Style.text.tutorial_header
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					fillMode: Image.PreserveAspectFit
					height: width * (sourceSize.height / sourceSize.width)
					source: "qrc:///images/tutorial/reader_sac_menu_%1_%2.svg".arg(Constants.layout).arg(SettingsModel.language)
					width: parent.width * 0.9
				}
			}
			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}
			Column {
				spacing: Constants.component_spacing
				width: parent.width

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					horizontalAlignment: Text.AlignRight

					//: LABEL ANDROID IOS
					text: qsTr("Now")
					textStyle: Style.text.tutorial_header

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
				GButton {
					id: pairingButton
					anchors.horizontalCenter: parent.horizontalCenter
					animationsDisabled: true

					//: LABEL ANDROID IOS
					text: qsTr("Pair device")

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					fillMode: Image.PreserveAspectFit
					height: width * (sourceSize.height / sourceSize.width)
					rotation: 180
					source: "qrc:///images/tutorial/up_icon.svg"
					width: parent.width * 0.1
				}
				Item {
					anchors.horizontalCenter: parent.horizontalCenter
					height: greyBackgroundRect.height
					width: pairingCodeText.width

					Rectangle {
						id: greyBackgroundRect
						anchors.horizontalCenter: parent.horizontalCenter
						color: Style.color.tutorial_box_background
						height: width
						width: pairingCodeText.width - 40
					}
					GText {
						id: pairingCodeText
						anchors.horizontalCenter: parent.horizontalCenter
						anchors.top: greyBackgroundRect.top
						horizontalAlignment: Text.AlignHCenter

						//: LABEL ANDROID IOS
						text: qsTr("Pairing code")
						textStyle: Style.text.tutorial_header_highlight
						topPadding: 30

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
					}
					GText {
						id: appearingText
						anchors.bottom: greyBackgroundRect.bottom
						anchors.horizontalCenter: parent.horizontalCenter
						bottomPadding: 30
						horizontalAlignment: Text.AlignHCenter

						//: LABEL ANDROID IOS
						text: qsTr("appears!")
						textStyle: Style.text.tutorial_header

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
					}
				}
			}
			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
				visible: Constants.is_layout_ios
			}
			Item {
				height: Math.max(noticeImage5.height, noticeText5.height)
				visible: Constants.is_layout_ios
				width: parent.width

				TutorialImage {
					id: noticeImage5
					centerX: 0.2
					centerY: 0.5
					source: "qrc:///images/tutorial/hint.svg"
					width: parent.width * 0.2
				}
				GText {
					id: noticeText5
					horizontalAlignment: Text.AlignLeft

					//: LABEL IOS
					text: qsTr("On the first use of the Smartphone as card reader (SaC), iOS will asks for your permission to access the local network. This permission is required in order find and connect to your SaC. After the first request you can always access the permission in the iOS settings for this app.")
					textStyle: Style.text.tutorial_content_highlight
					width: parent.width * 0.6
					x: (parent.width * 0.65) - (width / 2)
					y: (parent.height * 0.5) - (height / 2)

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
			}
			Image {
				anchors.horizontalCenter: parent.horizontalCenter
				fillMode: Image.PreserveAspectFit
				height: width * (sourceSize.height / sourceSize.width)
				rotation: 90
				source: "qrc:///images/tutorial/arrow_blue.svg"
				width: parent.width * 0.2
			}
			Column {
				spacing: Constants.component_spacing
				width: parent.width

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					fillMode: Image.PreserveAspectFit
					height: width * (sourceSize.height / sourceSize.width)
					source: "qrc:///images/tutorial/reader_sac_no_nfc_devices.svg"
					width: parent.width * 0.5
				}
				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					horizontalAlignment: Text.AlignHCenter
					text: (Constants.is_layout_ios ?
						//: LABEL IOS
						qsTr("Now open the AusweisApp2 on your device <b>without</b> NFC and select <b>Manage pairings</b>.") :
						//: LABEL ANDROID
						qsTr("Now open the AusweisApp2 on your device <b>without</b> NFC and select <b>Smartphone as card reader</b>."))
					textStyle: Style.text.tutorial_header
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					horizontalAlignment: Text.AlignHCenter

					//: LABEL ANDROID IOS
					text: qsTr("Now select <b>Settings</b>.")
					textStyle: Style.text.tutorial_header
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
			}
			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}
			Column {
				spacing: Constants.component_spacing
				width: parent.width

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					fillMode: Image.PreserveAspectFit
					height: width * (sourceSize.height / sourceSize.width)
					source: "qrc:///images/tutorial/phone_list.svg"
					width: parent.width * 0.5
				}
				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					horizontalAlignment: Text.AlignHCenter

					//: LABEL ANDROID IOS
					text: qsTr("Choose smartphone from list")
					textStyle: Style.text.tutorial_header
					width: parent.width * 0.7

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
				Item {
					height: noticeImage4.height
					width: parent.width

					TutorialImage {
						id: noticeImage4
						centerX: 0.2
						centerY: 0.5
						source: "qrc:///images/tutorial/hint.svg"
						width: parent.width * 0.2
					}
					GText {
						horizontalAlignment: Text.AlignLeft

						//: LABEL ANDROID IOS
						text: qsTr("Enter pairing code next.")
						textStyle: Style.text.tutorial_content_highlight
						width: parent.width * 0.6
						x: (parent.width * 0.65) - (width / 2)
						y: (parent.height * 0.5) - (height / 2)

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
					}
				}
			}
			Image {
				anchors.horizontalCenter: parent.horizontalCenter
				fillMode: Image.PreserveAspectFit
				height: width * (sourceSize.height / sourceSize.width)
				rotation: 90
				source: "qrc:///images/tutorial/arrow_blue.svg"
				width: parent.width * 0.2
			}
			Column {
				width: parent.width

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					fillMode: Image.PreserveAspectFit
					height: width * (sourceSize.height / sourceSize.width)
					source: "qrc:///images/tutorial/reader_sac_no_nfc_provider.svg"
					width: parent.width * 0.5
				}
				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					horizontalAlignment: Text.AlignHCenter

					//: LABEL ANDROID IOS
					text: qsTr("Click link on the website of the provider on the device <b>without</b> NFC.")
					textStyle: Style.text.tutorial_header_secondary
					width: parent.width * 0.6

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
			}
			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}
			Item {
				height: userdataExample.height + textAccessWhoWhat.height + textOpenAutomatic.height + 2 * Constants.component_spacing
				width: parent.width

				TutorialImage {
					centerX: 0.2
					centerY: (textAccessWhoWhat.y + textAccessWhoWhat.height) / (2 * parent.height)
					source: "qrc:///images/tutorial/tablet-no-nfc.svg"
					width: parent.width * 0.15
				}
				GText {
					id: textOpenAutomatic
					horizontalAlignment: Text.AlignHCenter
					rightPadding: Constants.component_spacing

					//: LABEL ANDROID IOS
					text: qsTr("The App opens automatically.")
					textStyle: Style.text.tutorial_content
					width: parent.width * 0.6

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					anchors {
						right: parent.right
						top: parent.top
					}
				}
				GText {
					id: textAccessWhoWhat
					horizontalAlignment: Text.AlignHCenter
					rightPadding: Constants.component_spacing

					//: LABEL ANDROID IOS
					text: qsTr("The AusweisApp2 will display who wants to access which data.")
					textStyle: Style.text.tutorial_header_secondary
					width: parent.width * 0.6

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					anchors {
						right: parent.right
						top: textOpenAutomatic.bottom
						topMargin: Constants.component_spacing
					}
				}
				TutorialImage {
					id: userdataExample
					source: "qrc:///images/tutorial/reader_nfc_userdata_example_%1.svg".arg(SettingsModel.language)
					width: parent.width * 0.8

					anchors {
						horizontalCenter: parent.horizontalCenter
						top: textAccessWhoWhat.bottom
						topMargin: Constants.component_spacing
					}
				}
			}
			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}
			Column {
				spacing: Constants.component_spacing
				width: parent.width

				GText {
					id: startProcessText
					anchors.horizontalCenter: parent.horizontalCenter
					//: LABEL ANDROID IOS
					text: qsTr("Start the process with a click on:")
					textStyle: Style.text.tutorial_header_secondary

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
				Row {
					anchors.horizontalCenter: parent.horizontalCenter
					height: authArrow.height
					spacing: Constants.component_spacing

					Image {
						id: authArrow
						fillMode: Image.PreserveAspectFit
						height: width * (sourceSize.height / sourceSize.width)
						source: "qrc:///images/tutorial/arrow_blue.svg"
						width: parent.width * 0.2
					}
					GButton {
						animationsDisabled: true
						icon.source: "qrc:///images/identify.svg"

						//: LABEL ANDROID IOS
						text: qsTr("Proceed to PIN entry")
						tintIcon: true

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
					}
				}
			}
			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}
			Column {
				spacing: Constants.component_spacing
				width: parent.width

				Item {
					height: screenshotAuth.height
					width: parent.width

					TutorialImage {
						id: screenshotAuth
						centerX: 0.5
						centerY: 0.4
						source: "qrc:///images/tutorial/screenshot_choose_reader_%1_%2.png".arg(Constants.layout).arg(SettingsModel.language)
						width: parent.width * 0.5
					}
					TutorialImage {
						id: pointerImage
						centerX: 0.52
						centerY: 0.89
						source: "qrc:///images/tutorial/hand.svg"
						width: parent.width * 0.1
					}
				}
				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					horizontalAlignment: Text.AlignHCenter

					//: LABEL ANDROID IOS
					text: qsTr("Tap on WiFi")
					textStyle: Style.text.tutorial_header
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
				}
			}
			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}
			Column {
				spacing: Constants.component_spacing
				width: parent.width

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					fillMode: Image.PreserveAspectFit
					height: width * (sourceSize.height / sourceSize.width)
					source: "qrc:///images/tutorial/where_lay_down_id.svg"
					width: parent.width * 0.8
				}
				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					horizontalAlignment: Text.AlignHCenter

					//: LABEL ANDROID IOS
					text: qsTr("... and place the ID card onto the NFC interface.")
					textStyle: Style.text.tutorial_header_secondary
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
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

						//: LABEL ANDROID IOS
						text: qsTr("Do not move device or ID card!")
						textStyle: Style.text.tutorial_content_highlight
						width: parent.width * 0.6
						x: (parent.width * 0.65) - (width / 2)
						y: (parent.height * 0.5) - (height / 2)

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
					}
				}
			}
			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}
			Item {
				height: Math.max(nfcPosition.height, nfcText.height)
				width: parent.width * 0.9

				TutorialImage {
					id: nfcPosition
					source: "qrc:///images/tutorial/reader_nfc_smartphone_nfc_position.svg"
					width: parent.width * 0.75

					anchors {
						left: parent.left
						verticalCenter: parent.verticalCenter
					}
				}
				GText {
					id: nfcText
					horizontalAlignment: Text.AlignLeft

					//: LABEL ANDROID IOS
					text: qsTr("The correct position is specific for your device. If a position does not work try a different one. The AusweisApp2 shows different common positions.")
					textStyle: Style.text.tutorial_content_highlight
					width: parent.width * 0.55

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					anchors {
						right: parent.right
						verticalCenter: parent.verticalCenter
					}
				}
			}
			GText {
				anchors.horizontalCenter: parent.horizontalCenter
				horizontalAlignment: Text.AlignHCenter

				//: LABEL ANDROID IOS
				text: qsTr("If your device is unable to detect your ID card try to check the device capabilities by clicking on \"Check device and ID card\" on the start page.")
				textStyle: Style.text.tutorial_content_highlight
				width: parent.width * 0.9

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
			}
			GText {
				anchors.horizontalCenter: parent.horizontalCenter
				horizontalAlignment: Text.AlignHCenter

				//: LABEL ANDROID IOS
				text: qsTr("You can find more information on compatible devices on our %1mobile device list%2.").arg("<a href=\"https://www.ausweisapp.bund.de/%1/aa2/mobile-devices\">".arg(SettingsModel.language)).arg("</a>")
				textStyle: Style.text.tutorial_header_secondary
				visible: Constants.is_layout_android
				width: parent.width * 0.9

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
			}
			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}
			Column {
				spacing: Constants.component_spacing
				width: parent.width

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
						source: "qrc:///images/tutorial/reader_nfc_pin6.svg"
						width: parent.width * 0.8
						z: 1
					}
					GText {
						horizontalAlignment: Text.AlignHCenter

						//: LABEL ANDROID IOS
						text: qsTr("six-digit PIN")
						textStyle: Style.text.tutorial_header_highlight
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
				Item {
					height: Math.max(noticeImage2.height, noticeText2.height)
					width: parent.width

					TutorialImage {
						id: noticeImage2
						centerX: 0.2
						centerY: 0.5
						source: "qrc:///images/tutorial/hint.svg"
						width: parent.width * 0.2
					}
					GText {
						id: noticeText2
						horizontalAlignment: Text.AlignLeft

						//: LABEL ANDROID IOS
						text: qsTr("This is only possible if you have exchanged the five-digit Transport PIN with a six-digit PIN beforehand.")
						textStyle: Style.text.tutorial_content_highlight
						width: parent.width * 0.6
						x: (parent.width * 0.65) - (width / 2)
						y: (parent.height * 0.5) - (height / 2)

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()
					}
				}
			}
			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}
			MouseArea {
				height: finishedButton.height
				width: parent.width

				onClicked: pop()

				Image {
					id: finishedButton
					anchors.horizontalCenter: parent.horizontalCenter
					fillMode: Image.PreserveAspectFit
					height: width * (sourceSize.height / sourceSize.width)
					source: "qrc:///images/tutorial/reader_nfc_finished.svg"
					width: parent.width * 0.8
				}
			}
			GSpacer {
				height: footer.height
				width: parent.width
			}
		}
	}
	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	Rectangle {
		anchors.fill: parent
		color: Constants.white
		z: -1
	}
	TutorialStatusBar {
		id: statusBar
	}
	TutorialReaderMethodFooter {
		id: footer
		width: baseItem.width

		onMenuClicked: pop()
		onQuitTutorialClicked: {
			pop();
			baseItem.quitTutorialClicked();
		}
	}
}
