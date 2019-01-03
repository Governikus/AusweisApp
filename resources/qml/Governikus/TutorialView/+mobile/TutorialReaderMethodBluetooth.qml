import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: baseItem
	leftTitleBarAction: TitleBarAction { state: "back"; onClicked: firePop() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Tutorial: Bluetooth") + settingsModel.translationTrigger; font.bold: true }

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
					text: qsTr("Using a bluetooth card reader") + settingsModel.translationTrigger
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
					radius: numberFour.height
					border.width: Utils.dp(3)
					border.color: Constants.tutorial_blue

					Text {
						id: numberFour
						anchors.centerIn: parent
						text: "4"
						font.family: "Noto Serif"
						font.pixelSize: Constants.tutorial_content_header_h2_font_size
						font.bold: true
						horizontalAlignment: Text.AlignHCenter
					}
				}

				Item {
					width: parent.width
					height: bluetoothImage.height

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
							source: "qrc:///images/tutorial/bluetooth.svg"
							width: parent.width * 0.8

							centerX: 0.5
							centerY: 0.5
						}
					}

					TutorialImage {
						id: bluetoothImage
						source: "qrc:///images/tutorial/generated/how_method_bluetooth.svg"
						width: parent.width

						centerX: 0.5
						centerY: 0.5
					}

					Text {
						width: parent.width * 0.4
						text: qsTr("App on smartphone or tablet") + settingsModel.translationTrigger
						horizontalAlignment: Text.AlignHCenter
						wrapMode: Text.WordWrap

						x: (parent.width * 0.25) - (width / 2)
						y: (parent.height * 0.95) - (height / 2)
					}

					Text {
						width: parent.width * 0.4
						text: qsTr("Bluetooth card reader") + settingsModel.translationTrigger
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
					width: parent.width * 0.8
					text: qsTr("You need a suitable card reader if you want to use the bluetooth connection.") + settingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					font.pixelSize: Constants.tutorial_content_header_h1_font_size
					font.family: "Noto Serif"
					wrapMode: Text.WordWrap
				}

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					source: "qrc:///images/tutorial/generated/reader_bluetooth_connection.svg"
					width: parent.width * 0.9
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				Item {
					width: parent.width
					height: Math.max(leftText.height, rightText.height)

					Text {
						id: leftText
						width: parent.width * 0.4
						anchors.left: parent.left
						leftPadding: Utils.dp(30)
						text: qsTr("Set the card reader visible first...") + settingsModel.translationTrigger
						horizontalAlignment: Text.AlignHCenter
						font.pixelSize: Constants.tutorial_content_header_h2_font_size
						font.family: "Noto Serif"
						wrapMode: Text.WordWrap
					}

					Text {
						id: rightText
						width: parent.width * 0.4
						anchors.right: parent.right
						rightPadding: Utils.dp(30)
						text: qsTr("... and then pair it with your device.") + settingsModel.translationTrigger
						horizontalAlignment: Text.AlignHCenter
						font.pixelSize: Constants.tutorial_content_header_h2_font_size
						font.family: "Noto Serif"
						wrapMode: Text.WordWrap
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
					source: "qrc:///images/tutorial/generated/reader_sac_no_nfc_provider.svg"
					width: parent.width * 0.5
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.6
					text: qsTr("Click the link on the website of the service provider.") + settingsModel.translationTrigger
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
					source: "qrc:///images/tutorial/tablet-no-nfc.svg"
					width: parent.width * 0.15

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

				Item {
					width: parent.width
					height: screenshotIdentify.height

					TutorialImage {
						id: screenshotIdentify
						source: qsTr("qrc:///images/tutorial/screenshot_choose_reader_en.png") + settingsModel.translationTrigger
						width: parent.width * 0.5

						centerX: 0.5
						centerY: 0.4
					}

					TutorialImage {
						id: pointerImage
						source: "qrc:///images/tutorial/hand.svg"
						width: parent.width * 0.1
						centerX: 0.53
						centerY: 0.87
					}
				}

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					text: qsTr("Tap on Bluetooth") + settingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					font.pixelSize: Constants.tutorial_content_header_h1_font_size
					font.family: "Noto Serif"
					wrapMode: Text.WordWrap
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Column {
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width
				spacing: Constants.component_spacing

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					text: qsTr("Insert card into card reader") + settingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					font.pixelSize: Constants.tutorial_content_header_h1_font_size
					font.family: "Noto Serif"
					wrapMode: Text.WordWrap
				}

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					source: "qrc:///images/tutorial/generated/reader_bluetooth_card_inserted.svg"
					width: parent.width * 0.3
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					text: qsTr("... and confirm the displayed information.") + settingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					font.pixelSize: Constants.tutorial_content_header_h1_font_size
					font.family: "Noto Serif"
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
						text: (settingsModel.language === "en" ? qsTr("now on the card reader!") : qsTr("enter on the card reader!")) + settingsModel.translationTrigger
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
					height: Math.max(noticeImage2.height, noticeText.height)

					TutorialImage {
						id: noticeImage2
						source: "qrc:///images/tutorial/hint.svg"

						width: parent.width * 0.2
						centerY: 0.5
						centerX: 0.2
					}

					Text {
						id: noticeText
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

