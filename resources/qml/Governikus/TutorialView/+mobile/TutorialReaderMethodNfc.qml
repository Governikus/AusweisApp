/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

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
	title: qsTr("Tutorial: NFC")

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

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("Direct connection via NFC chip")
					textStyle: Style.text.tutorial_header_secondary
					font.bold: true
					horizontalAlignment: Text.AlignHCenter
				}

				Rectangle {
					anchors.horizontalCenter: parent.horizontalCenter
					height: radius * 2
					width: radius * 2
					radius: numberOne.height
					border.width: 3
					border.color: Style.color.tutorial_how

					GText {
						id: numberOne
						anchors.centerIn: parent

						Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
						Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

						text: "1"
						textStyle: Style.text.tutorial_header_secondary
						font.bold: true
						horizontalAlignment: Text.AlignHCenter
					}
				}

				Image {
					source: "qrc:///images/tutorial/how_method_nfc.svg"
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.8
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.6

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL IOS
					text: (Constants.is_layout_ios ? qsTr("App on iPhone <b>with</b> NFC chip as card reader")
												//: LABEL ANDROID
												: qsTr("App on Android smartphone <b>with</b> NFC chip as card reader"))
					textStyle: Style.text.tutorial_content
					horizontalAlignment: Text.AlignHCenter
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

			Item {
				width: parent.width
				height: providerOnSmartphone.height

				TutorialImage {
					id: providerOnSmartphone
					source: "qrc:///images/tutorial/reader_nfc_provider_on_smartphone.svg"
					width: parent.width
				}

				GText {
					width: parent.width * 0.6

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("Click link on the website of the provider.")
					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_header_secondary

					x: (parent.width * 0.5) - (width / 2)
					y: (parent.height * 0.9) - (height / 2)
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Item{
				width: parent.width
				height: userdataExample.height + textAccessWhoWhat.height + textOpenAutomatic.height + 3 * Constants.component_spacing

				TutorialImage {
					source: "qrc:///images/tutorial/reader_nfc_npa_on_smartphone.svg"
					width: parent.width * 0.3

					centerX: 0.2
					centerY: 0.2
				}

				TutorialImage {
					id: userdataExample
					source: "qrc:///images/tutorial/reader_nfc_userdata_example_%1.svg".arg(SettingsModel.language)
					width: parent.width * 0.8

					centerX: 0.5
					centerY: 0.75
				}

				GText {
					id: textOpenAutomatic
					width: parent.width * 0.6
					rightPadding: Constants.component_spacing

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("The App opens automatically.")
					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_content

					x: (parent.width * 0.7) - (width / 2)
					y: (parent.height * 0) - (height / 2)
				}

				GText {
					id: textAccessWhoWhat
					width: parent.width * 0.6
					rightPadding: Constants.component_spacing

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID IOS
					text: qsTr("The AusweisApp2 will display who wants to access which data.")
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

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					textStyle: Style.text.tutorial_header_secondary
					//: LABEL ANDROID IOS
					text: qsTr("Start the process with a click on:")
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
				anchors.horizontalCenter: parent.horizontalCenter
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

					//: LABEL IOS
					text: (Constants.is_layout_ios ? qsTr("... and place the top of the iPhone onto the ID card.")
													//: LABEL ANDROID
													: qsTr("... and place the ID card flat onto the NFC interface."))
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
				visible: Constants.is_layout_android
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Item {
				width: parent.width
				height: nfcPosition.height + 2 * Constants.component_spacing
				visible: Constants.is_layout_android

				TutorialImage {
					id: nfcPosition
					source: "qrc:///images/tutorial/reader_nfc_smartphone_nfc_position.svg"

					width: parent.width * 0.7
					centerY: 0.5
					centerX: 0.35
				}

				GText {
					width: parent.width * 0.5
					rightPadding: Constants.component_spacing

					Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
					Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

					//: LABEL ANDROID
					text: qsTr("The correct position is specific for your device. If a position does not work try a different one. The AusweisApp2 shows different common positions. If your device is unable to detect your ID card try to check the device capabilities by clicking on \"Check device and ID card\" on the start page.")
					textStyle: Style.text.tutorial_content
					font.bold: true
					horizontalAlignment: Text.AlignLeft

					x: (parent.width * 0.75) - (width / 2)
					y: (parent.height * 0.5) - (height / 2)
				}
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

			Image {
				width: parent.width * 0.7
				height: width * (sourceSize.height / sourceSize.width)
				anchors.horizontalCenter: parent.horizontalCenter

				source: "qrc:///images/tutorial/play_movie.png"
				fillMode: Image.PreserveAspectFit

				MouseArea {
					anchors.fill: parent

					//: LABEL ANDROID IOS
					Accessible.name: qsTr("Open YouTube video")
					Accessible.role: Accessible.Button
					Accessible.onPressAction: if (Qt.platform.os === "ios") clicked(null)

					onClicked: Constants.is_layout_ios ? Qt.openUrlExternally("https://www.youtube.com/watch?v=qArkIGs0cFM&list=PLLB5ERhVkn25qQXgMHQr-1KgyZsJKoSAm&index=1")
														: Qt.openUrlExternally("https://www.youtube.com/watch?v=OtduiptNKQI&list=PLLB5ERhVkn25qQXgMHQr-1KgyZsJKoSAm&index=2")
				}
			}

			GText {
				width: parent.width * 0.9
				anchors.horizontalCenter: parent.horizontalCenter

				Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
				Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

				//: LABEL ANDROID IOS
				text: qsTr("You can also watch this YouTube video explaining the process.")
				textStyle: Style.text.tutorial_header_secondary
				horizontalAlignment: Text.AlignHCenter
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			MouseArea {
				width: parent.width
				height: finishedButton.height

				Image {
					id: finishedButton
					width: parent.width * 0.8
					height: width * (sourceSize.height / sourceSize.width)

					anchors.horizontalCenter: parent.horizontalCenter

					source: "qrc:///images/tutorial/reader_nfc_finished.svg"
					fillMode: Image.PreserveAspectFit
				}

				onClicked: firePop()
			}

			GSpacer {
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
