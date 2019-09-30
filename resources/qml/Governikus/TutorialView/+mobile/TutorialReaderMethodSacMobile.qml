/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: baseItem

	titleBarVisible: false
	automaticSafeAreaMarginHandling: false
	navigationAction: NavigationAction { state: "back"; onClicked: firePop() }
	//: LABEL ANDROID IOS
	title: qsTr("Tutorial: Smartphone as card reader") + SettingsModel.translationTrigger

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

			Item {
				id: statusBarSpacer
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
						source: "qrc:///images/tutorial/generated/how_method_sac_mobile.svg"
						width: parent.width

						centerX: 0.5
						centerY: 0.5
					}

					GText {
						width: parent.width * 0.4
						//: LABEL ANDROID IOS
						text: qsTr("App on tablet or smartphone <b>without</b> NFC chip") + SettingsModel.translationTrigger
						textStyle: Style.text.tutorial_content
						horizontalAlignment: Text.AlignHCenter

						x: (parent.width * 0.25) - (width / 2)
						y: (parent.height * 0.95) - (height / 2)
					}

					GText {
						width: parent.width * 0.4
						//: LABEL ANDROID IOS
						text: qsTr("Smartphone <b>with</b> NFC chip as card reader") + SettingsModel.translationTrigger
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
					//: LABEL ANDROID IOS
					text: qsTr("Install AusweisApp2 on both your device without NFC <b>and</b> your smartphone with NFC capability.") + SettingsModel.translationTrigger
					textStyle: Style.text.tutorial_header
					horizontalAlignment: Text.AlignHCenter
				}

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.8
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
					source: "qrc:///images/tutorial/generated/reader_sac_aa2_ok.svg"
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
					//: LABEL ANDROID IOS
					text: qsTr("Both devices have to be connected to the same wifi network") + SettingsModel.translationTrigger
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
					//: LABEL ANDROID IOS
					text: (Constants.is_layout_ios ? qsTr("Now choose \"Remote\" in the AusweisApp2 on your smartphone...")
													: qsTr("Now choose \"Smartphone as card reader\" in the AusweisApp2 on your smartphone..."))
													+ SettingsModel.translationTrigger
					textStyle: Style.text.tutorial_header
					horizontalAlignment: Text.AlignHCenter
				}

				Image {
					//: LABEL ANDROID IOS
					source: qsTr("qrc:///images/tutorial/generated/reader_sac_menu_%1_en.svg").arg(Constants.layout) + SettingsModel.translationTrigger
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
					//: LABEL ANDROID IOS
					text: qsTr("Now") + SettingsModel.translationTrigger
					textStyle: Style.text.tutorial_header
					horizontalAlignment: Text.AlignRight
				}

				GButton {
					id: remoteButton
					buttonColor: Constants.green
					anchors.horizontalCenter: parent.horizontalCenter
					//: LABEL ANDROID IOS
					text: qsTr("Start remote service") + SettingsModel.translationTrigger
					animationsDisabled: true
				}

				Image {
					source: "qrc:///images/tutorial/up_icon.svg"
					anchors.horizontalCenter: parent.horizontalCenter
					rotation: 180
					width: parent.width * 0.1
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					//: LABEL ANDROID IOS
					text: qsTr("Next") + SettingsModel.translationTrigger
					textStyle: Style.text.tutorial_header
					horizontalAlignment: Text.AlignRight
				}

				GButton {
					id: pairingButton
					anchors.horizontalCenter: parent.horizontalCenter
					//: LABEL ANDROID IOS
					text: qsTr("Start pairing") + SettingsModel.translationTrigger
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
						//: LABEL ANDROID IOS
						text: qsTr("Pairing code") + SettingsModel.translationTrigger
						textStyle: Style.text.tutorial_header
						font.bold: true
						horizontalAlignment: Text.AlignHCenter
					}

					GText {
						id: appearingText
						anchors.bottom: greyBackgroundRect.bottom
						bottomPadding: 30
						anchors.horizontalCenter: parent.horizontalCenter
						//: LABEL ANDROID IOS
						text: qsTr("appears!") + SettingsModel.translationTrigger
						textStyle: Style.text.tutorial_header
						horizontalAlignment: Text.AlignHCenter
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

				Image {
					source: "qrc:///images/tutorial/generated/reader_sac_no_nfc_devices.svg"
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.5
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					//: LABEL ANDROID IOS
					text: (Constants.is_layout_ios ? qsTr("Now open the AusweisApp2 on your device <b>without</b> NFC and select <b>Configure remote service</b>.")
													: qsTr("Now open the AusweisApp2 on your device <b>without</b> NFC and select <b>Smartphone as card reader</b>."))
													+ SettingsModel.translationTrigger
					textStyle: Style.text.tutorial_header
					horizontalAlignment: Text.AlignHCenter
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					//: LABEL ANDROID IOS
					text: qsTr("Now select <b>Settings</b>.") + SettingsModel.translationTrigger
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
					//: LABEL ANDROID IOS
					text: qsTr("Choose smartphone from list") + SettingsModel.translationTrigger
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
						//: LABEL ANDROID IOS
						text: qsTr("Enter pairing code next.") + SettingsModel.translationTrigger
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
					source: "qrc:///images/tutorial/generated/reader_sac_no_nfc_provider.svg"
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
					width: parent.width * 0.5
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.6
					//: LABEL ANDROID IOS
					text: qsTr("Click link on the website of the service provider on the device <b>without</b> NFC.") + SettingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_header_secondary
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Item{
				width: parent.width
				height: userdataExample.height + textAccessWhoWhat.height + textOpenAutomatic.height + 3 * Constants.component_spacing

				TutorialImage {
					source: "qrc:///images/tutorial/tablet-no-nfc.svg"
					width: parent.width * 0.15

					centerX: 0.2
					centerY: 0.2
				}

				TutorialImage {
					id: userdataExample
					//: LABEL ANDROID IOS
					source: qsTr("qrc:///images/tutorial/generated/reader_nfc_userdata_example_en.svg") + SettingsModel.translationTrigger
					width: parent.width * 0.8

					centerX: 0.5
					centerY: 0.75
				}

				GText {
					id: textOpenAutomatic
					width: parent.width * 0.6
					//: LABEL ANDROID IOS
					text: qsTr("The App opens automatically.") + SettingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_content

					x: (parent.width * 0.7) - (width / 2)
					y: (parent.height * 0) - (height / 2)
				}

				GText {
					id: textAccessWhoWhat
					width: parent.width * 0.6
					//: LABEL ANDROID IOS
					text: qsTr("The AusweisApp2 will display who wants to access which data.") + SettingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_header_secondary

					x: (parent.width * 0.7) - (width / 2)
					y: (parent.height * 0.2) - (height / 2)
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
					textStyle: Style.text.tutorial_header_secondary
					//: LABEL ANDROID IOS
					text: qsTr("Start the process with a click on:") + SettingsModel.translationTrigger
				}

				Row {
					anchors.horizontalCenter: parent.horizontalCenter
					height: identifyArrow.height
					spacing: Constants.component_spacing

					Image {
						id: identifyArrow
						source: "qrc:///images/tutorial/arrow_blue.svg"
						width: parent.width * 0.2
						height: width * (sourceSize.height / sourceSize.width)
						fillMode: Image.PreserveAspectFit
					}

					GButton {
						id: identifyButton
						iconSource: "qrc:///images/npa.svg"
						//: LABEL ANDROID IOS
						text: qsTr("Proceed to PIN entry") + SettingsModel.translationTrigger;
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
					height: screenshotIdentify.height

					TutorialImage {
						id: screenshotIdentify
						//: LABEL ANDROID IOS
						source: qsTr("qrc:///images/tutorial/screenshot_choose_reader_%1_en.png").arg(Constants.layout) + SettingsModel.translationTrigger
						width: parent.width * 0.5

						centerX: 0.5
						centerY: 0.4
					}

					TutorialImage {
						id: pointerImage
						source: "qrc:///images/tutorial/hand.svg"
						width: parent.width * 0.1
						centerX: 0.43
						centerY: 0.87
					}
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					//: LABEL ANDROID IOS
					text: qsTr("Tap on Wifi") + SettingsModel.translationTrigger
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
					//: LABEL ANDROID IOS
					source: qsTr( "qrc:///images/tutorial/generated/where_lay_down_id.svg") + SettingsModel.translationTrigger
					width: parent.width * 0.8
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					//: LABEL ANDROID IOS
					text: qsTr("... and place the id card onto the NFC interface.") + SettingsModel.translationTrigger

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
						//: LABEL ANDROID IOS
						text: qsTr("Do not move device or id card!") + SettingsModel.translationTrigger
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
				width: parent.width
				height: nfcPosition.height + 2 * Constants.component_spacing

				TutorialImage {
					id: nfcPosition
					source: "qrc:///images/tutorial/generated/reader_nfc_smartphone_nfc_position.svg"

					width: parent.width * 0.7
					centerY: 0.5
					centerX: 0.35
				}

				GText {
					width: parent.width * 0.5
					//: LABEL ANDROID IOS
					text: qsTr("The correct position is specific for your device...") + SettingsModel.translationTrigger
					textStyle: Style.text.tutorial_content
					font.bold: true
					horizontalAlignment: Text.AlignLeft

					x: (parent.width * 0.75) - (width / 2)
					y: (parent.height * 0.4) - (height / 2)
				}

				GText {
					width: parent.width * 0.5
					//: LABEL ANDROID IOS
					text: "<a href=\"%1\">%2</a>".arg(qsTr("https://www.ausweisapp.bund.de/mobile-geraete/")).arg(qsTr("To mobile devices"))  + SettingsModel.translationTrigger
					textStyle: Style.text.tutorial_header_secondary
					font.bold: true
					font.underline: true
					horizontalAlignment: Text.AlignLeft
					onLinkActivated: Qt.openUrlExternally(link)

					x: (parent.width * 0.75) - (width / 2)
					y: (parent.height * 0.7) - (height / 2)
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
						source: "qrc:///images/tutorial/generated/reader_nfc_pin6.svg"
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
						//: LABEL ANDROID IOS
						text: qsTr("This is only possible if you have exchanged the 5 digits long transport PIN with a 6 digits long personal PIN beforehand.") + SettingsModel.translationTrigger
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
					source: "qrc:///images/tutorial/generated/reader_nfc_finished.svg"
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.8
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				onClicked: firePop()
			}

			Item {
				id: footerSpacer
				width: parent.width
				height: footer.height
			}
		}
	}

	TutorialStatusBar {
		id: statusBar

		shaderSource: sectionPageFlickable
	}

	TutorialReaderMethodFooter {
		id: footer
		width: baseItem.width

		shaderSource: sectionPageFlickable
		onMenuClicked: firePop()
		onQuitTutorialClicked: {
			firePop()
			baseItem.quitTutorialClicked()
		}
	}
}
