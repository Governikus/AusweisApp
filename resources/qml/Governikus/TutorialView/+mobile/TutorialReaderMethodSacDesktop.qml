import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: baseItem
	leftTitleBarAction: TitleBarAction { state: "back"; onClicked: firePop() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Tutorial: Smartphone as card reader") + settingsModel.translationTrigger; font.bold: true }

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

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					text: qsTr("Smartphone as card reader") + settingsModel.translationTrigger
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h2_font_size
					font.bold: true
					horizontalAlignment: Text.AlignHCenter
					wrapMode: Text.WordWrap
				}

				Rectangle {
					anchors.horizontalCenter: parent.horizontalCenter
					height: radius * 2
					width: radius * 2
					radius: numberTwo.height
					border.width: Utils.dp(3)
					border.color: Constants.tutorial_blue

					Text {
						id: numberTwo
						anchors.centerIn: parent
						text: "2"
						font.family: "Noto Serif"
						font.pixelSize: Constants.tutorial_content_header_h2_font_size
						font.bold: true
						horizontalAlignment: Text.AlignHCenter
					}
				}

				Item {
					width: parent.width
					height: sacStationaryImage.height

					Rectangle {
						anchors.horizontalCenter: parent.horizontalCenter
						height: radius * 2
						width: radius * 2
						radius: parent.width * 0.06
						border.width: Utils.dp(3)
						border.color: Constants.tutorial_blue

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
						id: sacStationaryImage
						source: "qrc:///images/tutorial/generated/how_method_sac_desktop.svg"
						width: parent.width

						centerX: 0.5
						centerY: 0.5
					}

					Text {
						width: parent.width * 0.4
						text: qsTr("App on computer <b>without</b> NFC chip") + settingsModel.translationTrigger
						horizontalAlignment: Text.AlignHCenter
						wrapMode: Text.WordWrap

						x: (parent.width * 0.25) - (width / 2)
						y: (parent.height * 0.95) - (height / 2)
					}

					Text {
						width: parent.width * 0.4
						text: qsTr("Android smartphone <b>with</b> NFC chip as card reader") + settingsModel.translationTrigger
						horizontalAlignment: Text.AlignHCenter
						wrapMode: Text.WordWrap

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

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					text: qsTr("Install AusweisApp2 on both your computer <b>and</b> your android smartphone with NFC ability.") + settingsModel.translationTrigger
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h1_font_size
					horizontalAlignment: Text.AlignHCenter
					wrapMode: Text.WordWrap
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

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					text: qsTr("Both devices have to be connected to the same wifi network") + settingsModel.translationTrigger
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h1_font_size
					font.bold: true
					horizontalAlignment: Text.AlignHCenter
					wrapMode: Text.WordWrap
				}

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.3
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
					source: "qrc:///images/tutorial/wifi.svg"
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Column {
				width: parent.width
				spacing: Constants.component_spacing

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					text: qsTr("Now enter \"Smartphone as card reader\" in the app on your android smartphone...") + settingsModel.translationTrigger
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h1_font_size
					horizontalAlignment: Text.AlignHCenter
					wrapMode: Text.WordWrap
				}

				Image {
					source: qsTr("qrc:///images/tutorial/generated/reader_sac_menu_en.svg") + settingsModel.translationTrigger
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

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					text: qsTr("Now") + settingsModel.translationTrigger
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h1_font_size
					horizontalAlignment: Text.AlignRight
					wrapMode: Text.WordWrap
				}

				GButton {
					id: remoteButton
					buttonColor: "green"
					anchors.horizontalCenter: parent.horizontalCenter
					text: qsTr("Start remote service") + settingsModel.translationTrigger
				}

				Image {
					source: "qrc:///images/tutorial/up_icon.svg"
					anchors.horizontalCenter: parent.horizontalCenter
					rotation: 180
					width: parent.width * 0.1
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					text: qsTr("Next") + settingsModel.translationTrigger
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h1_font_size
					horizontalAlignment: Text.AlignRight
					wrapMode: Text.WordWrap
				}

				GButton {
					id: pairingButton
					anchors.horizontalCenter: parent.horizontalCenter
					text: qsTr("Start pairing") + settingsModel.translationTrigger
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
						width: pairingCodeText.width - Utils.dp(40)
						height: width
						color: Constants.tutorial_very_light_grey
					}

					Text {
						id: pairingCodeText
						anchors.horizontalCenter: parent.horizontalCenter
						anchors.top: greyBackgroundRect.top
						topPadding: Utils.dp(30)
						text: qsTr("Pairing code") + settingsModel.translationTrigger
						font.family: "Noto Serif"
						font.pixelSize: Constants.tutorial_content_header_h1_font_size
						font.bold: true
						horizontalAlignment: Text.AlignHCenter
						wrapMode: Text.WordWrap
					}

					Text {
						id: appearingText
						anchors.bottom: greyBackgroundRect.bottom
						bottomPadding: Utils.dp(30)
						anchors.horizontalCenter: parent.horizontalCenter
						text: qsTr("appears!") + settingsModel.translationTrigger
						font.family: "Noto Serif"
						font.pixelSize: Constants.tutorial_content_header_h1_font_size
						horizontalAlignment: Text.AlignHCenter
						wrapMode: Text.WordWrap
					}
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
					source: "qrc:///images/tutorial/generated/reader_sac_npa_on_laptop.svg"
					width: parent.width * 0.3
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.8
					text: qsTr("Start the App now on your computer and enter the settings.") + settingsModel.translationTrigger
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h1_font_size
					horizontalAlignment: Text.AlignHCenter
					wrapMode: Text.WordWrap
				}

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.8
					text: qsTr("Select the <b>Card Readers</b> tab.") + settingsModel.translationTrigger
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h1_font_size
					horizontalAlignment: Text.AlignHCenter
					wrapMode: Text.WordWrap
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
					height: desktopPairing.height

					TutorialImage {
						id: desktopPairing
						source: qsTr("qrc:///images/tutorial/screenshot_pairing_en.png") + settingsModel.translationTrigger

						width: parent.width * 0.6
					}
				}

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.8
					text: qsTr("Select smartphone from list and click \"pair\"") + settingsModel.translationTrigger
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h1_font_size
					horizontalAlignment: Text.AlignHCenter
					wrapMode: Text.WordWrap
				}

				Item {
					width: parent.width
					height: noticeImage3.height

					TutorialImage {
						id: noticeImage3
						source: "qrc:///images/tutorial/hint.svg"

						width: parent.width * 0.2
						centerY: 0.5
						centerX: 0.2
					}

					Text {
						width: parent.width * 0.6
						text: qsTr("Enter pairing code next.") + settingsModel.translationTrigger
						font.family: "Noto Serif"
						font.pixelSize: Constants.tutorial_content_font_size
						font.bold: true
						horizontalAlignment: Text.AlignLeft
						wrapMode: Text.WordWrap

						x: (parent.width * 0.65) - (width / 2)
						y: (parent.height * 0.5) - (height / 2)
					}
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Column {
				width: parent.width

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					source: "qrc:///images/tutorial/generated/reader_sac_provider_on_laptop.svg"
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
					width: parent.width * 0.5
				}

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.6
					text: qsTr("Click link on the website of the service provider.") + settingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					font.pixelSize: Constants.tutorial_content_header_h2_font_size
					font.family: "Noto Serif"
					wrapMode: Text.WordWrap
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Item{
				width: parent.width
				height: userdataExample.height + textAccessWhoWhat.height + textOpenAutomatic.height + 3 * Constants.component_spacing

				TutorialImage {
					source: "qrc:///images/tutorial/generated/reader_sac_npa_on_laptop.svg"
					width: parent.width * 0.3

					centerX: 0.2
					centerY: 0.2
				}

				TutorialImage {
					id: userdataExample
					source: qsTr("qrc:///images/tutorial/generated/reader_nfc_userdata_example_en.svg") + settingsModel.translationTrigger
					width: parent.width * 0.8

					centerX: 0.5
					centerY: 0.75
				}

				Text {
					id: textOpenAutomatic
					width: parent.width * 0.6
					text: qsTr("The App opens automatically.") + settingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					font.pixelSize: Constants.tutorial_content_font_size
					font.family: "Noto Serif"
					wrapMode: Text.WordWrap

					x: (parent.width * 0.7) - (width / 2)
					y: (parent.height * 0) - (height / 2)
				}

				Text {
					id: textAccessWhoWhat
					width: parent.width * 0.6
					text: qsTr("The AusweisApp2 will display who wants to access which data.") + settingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					font.pixelSize: Constants.tutorial_content_header_h2_font_size
					font.family: "Noto Serif"
					wrapMode: Text.WordWrap

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

				Text {
					id: startProcessText
					anchors.horizontalCenter: parent.horizontalCenter
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h2_font_size
					text: qsTr("Start the process with a click on:") + settingsModel.translationTrigger
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
						text: qsTr("Identify now") + settingsModel.translationTrigger;
					}
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
					source: qsTr( "qrc:///images/tutorial/generated/where_lay_down_id.svg") + settingsModel.translationTrigger
					width: parent.width * 0.8
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					text: qsTr("... and place the id card flat onto the NFC interface.") + settingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					font.pixelSize: Constants.tutorial_content_header_h2_font_size
					font.family: "Noto Serif"
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
						text: qsTr("Do not move device or id card!") + settingsModel.translationTrigger
						font.family: "Noto Serif"
						font.pixelSize: Constants.tutorial_content_font_size
						font.bold: true
						horizontalAlignment: Text.AlignLeft
						wrapMode: Text.WordWrap

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

				Text {
					width: parent.width * 0.5
					text: qsTr("The correct position is specific for your device...") + settingsModel.translationTrigger
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_font_size
					font.bold: true
					horizontalAlignment: Text.AlignLeft
					wrapMode: Text.WordWrap

					x: (parent.width * 0.75) - (width / 2)
					y: (parent.height * 0.4) - (height / 2)
				}

				Text {
					width: parent.width * 0.5
					text: "<a href=\"%1\">%2</a>".arg(qsTr("https://www.ausweisapp.bund.de/mobile-geraete/")).arg(qsTr("To mobile devices"))  + settingsModel.translationTrigger
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h2_font_size
					font.bold: true
					font.underline: true
					horizontalAlignment: Text.AlignLeft
					wrapMode: Text.WordWrap
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
						source: "qrc:///images/tutorial/generated/reader_nfc_pin6.svg"
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

					Text {
						id: noticeText2
						width: parent.width * 0.6
						text: qsTr("This is only possible if you have exchanged the 5 digits long transport PIN with a 6 digits long personal PIN beforehand.") + settingsModel.translationTrigger
						font.family: "Noto Serif"
						font.pixelSize: Constants.tutorial_content_font_size
						font.bold: true
						horizontalAlignment: Text.AlignLeft
						wrapMode: Text.WordWrap

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

	TutorialReaderMethodFooter {
		id: footer
		width: baseItem.width

		onMenuClicked: firePop()
		onQuitTutorialClicked: {
			firePop()
			baseItem.quitTutorialClicked()
		}
	}
}
