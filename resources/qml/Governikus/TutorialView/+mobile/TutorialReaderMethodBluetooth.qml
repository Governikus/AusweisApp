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
	title: qsTr("Tutorial: Bluetooth") + SettingsModel.translationTrigger

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

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					//: LABEL ANDROID IOS
					text: qsTr("Using a bluetooth card reader") + SettingsModel.translationTrigger
					textStyle: Style.text.tutorial_header_secondary
					font.bold: true
					horizontalAlignment: Text.AlignHCenter
				}

				Rectangle {
					anchors.horizontalCenter: parent.horizontalCenter
					height: radius * 2
					width: radius * 2
					radius: numberFour.height
					border.width: 3
					border.color: Style.color.tutorial_how

					GText {
						id: numberFour
						anchors.centerIn: parent
						text: "4"
						textStyle: Style.text.tutorial_header_secondary
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
						border.width: 3
						border.color: Style.color.tutorial_how

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

					GText {
						width: parent.width * 0.4
						//: LABEL ANDROID IOS
						text: qsTr("App on smartphone or tablet") + SettingsModel.translationTrigger
						textStyle: Style.text.tutorial_content
						horizontalAlignment: Text.AlignHCenter

						x: (parent.width * 0.25) - (width / 2)
						y: (parent.height * 0.95) - (height / 2)
					}

					GText {
						width: parent.width * 0.4
						//: LABEL ANDROID IOS
						text: qsTr("Bluetooth card reader") + SettingsModel.translationTrigger
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
					width: parent.width * 0.8
					//: LABEL ANDROID IOS
					text: qsTr("You need a suitable card reader if you want to use the bluetooth connection.") + SettingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_header
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

					GText {
						id: leftText
						width: parent.width * 0.4
						anchors.left: parent.left
						leftPadding: 30
						//: LABEL ANDROID IOS
						text: qsTr("Set the card reader visible first...") + SettingsModel.translationTrigger
						horizontalAlignment: Text.AlignHCenter
						textStyle: Style.text.tutorial_header_secondary
					}

					GText {
						id: rightText
						width: parent.width * 0.4
						anchors.right: parent.right
						rightPadding: 30
						//: LABEL ANDROID IOS
						text: qsTr("... and then pair it with your device.") + SettingsModel.translationTrigger
						horizontalAlignment: Text.AlignHCenter
						textStyle: Style.text.tutorial_header_secondary
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

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.6
					//: LABEL ANDROID IOS
					text: qsTr("Click the link on the website of the service provider.") + SettingsModel.translationTrigger
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
						source: qsTr("qrc:///images/tutorial/screenshot_choose_reader_%1_en.png").arg(Constants.layout) + SettingsModel.translationTrigger
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

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					//: LABEL ANDROID IOS
					text: qsTr("Tap on Bluetooth") + SettingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_header
				}
			}

			TutorialSeperator {
				source: "qrc:///images/tutorial/section_seperator_how.svg"
			}

			Column {
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width
				spacing: Constants.component_spacing

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					//: LABEL ANDROID IOS
					text: qsTr("Insert card into card reader") + SettingsModel.translationTrigger
					horizontalAlignment: Text.AlignHCenter
					textStyle: Style.text.tutorial_header
				}

				Image {
					anchors.horizontalCenter: parent.horizontalCenter
					source: "qrc:///images/tutorial/generated/reader_bluetooth_card_inserted.svg"
					width: parent.width * 0.3
					height: width * (sourceSize.height / sourceSize.width)
					fillMode: Image.PreserveAspectFit
				}

				GText {
					anchors.horizontalCenter: parent.horizontalCenter
					width: parent.width * 0.9
					//: LABEL ANDROID IOS
					text: qsTr("... and confirm the displayed information.") + SettingsModel.translationTrigger
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
						text: (SettingsModel.language === "en" ? qsTr("now on the card reader!") : qsTr("enter on the card reader!")) + SettingsModel.translationTrigger
						textStyle: Style.text.tutorial_header
						horizontalAlignment: Text.AlignHCenter

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

					GText {
						id: noticeText
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

