/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


TutorialContent {
	id: baseItem

	signal firePush(var pSectionPage)
	signal quitTutorialClicked()

	Item {
		Component {
			id: readerMethodNfc

			TutorialReaderMethodNfc {
				onQuitTutorialClicked:  baseItem.quitTutorialClicked()
			}
		}

		Component {
			id: readerMethodSacMobile

			TutorialReaderMethodSacMobile {
				onQuitTutorialClicked:  baseItem.quitTutorialClicked()
			}
		}

		Component {
			id: readerMethodSacDesktop

			TutorialReaderMethodSacDesktop {
				onQuitTutorialClicked:  baseItem.quitTutorialClicked()
			}
		}

		Component {
			id: readerMethodBluetooth

			TutorialReaderMethodBluetooth {
				onQuitTutorialClicked:  baseItem.quitTutorialClicked()
			}
		}
	}


	GText {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9
		//: INFO ANDROID IOS
		text: (Constants.is_layout_ios ? qsTr("How can I use the AusweisApp2 on my iPhone?")
										: qsTr("How can I use the AusweisApp2 on my smartphone?"))
										+ SettingsModel.translationTrigger
		textStyle: Style.text.tutorial_header
		font.italic: true
		horizontalAlignment: Text.AlignHCenter
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		source: "qrc:///images/tutorial/generated/how_questions_everywhere.svg"
		width: parent.width * 0.9
		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}

	Column {
		width: parent.width
		spacing: Constants.component_spacing

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
				//: INFO ANDROID IOS
				text: (Constants.is_layout_ios ? qsTr("Many iPhones (iPhone 7 and newer) can access the id card via the built-in NFC interface.")
												: qsTr("Many Android devices can access the id card via the built-in NFC interface."))
												+ SettingsModel.translationTrigger
				textStyle: Style.text.tutorial_content
				font.bold: true
				horizontalAlignment: Text.AlignLeft

				x: (parent.width * 0.65) - (width / 2)
				y: (parent.height * 0.5) - (height / 2)
			}
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			visible: Constants.is_layout_android
			//: LABEL ANDROID IOS
			text: qsTr("You can find a list of compatible NFC-capable smartphones here:") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header_secondary
			horizontalAlignment: Text.AlignHCenter
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			visible: Constants.is_layout_android
			//: LABEL ANDROID IOS
			text: "<a href=\"%1\">%1</a>".arg(qsTr("https://www.ausweisapp.bund.de/mobile-geraete/")) + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_content
			horizontalAlignment: Text.AlignHCenter
			onLinkActivated: Qt.openUrlExternally(link)
		}

		Image {
			source: "qrc:///images/tutorial/generated/how_device_lineup.svg"
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width
			height: width * (sourceSize.height / sourceSize.width)
			fillMode: Image.PreserveAspectFit
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("The AusweisApp2 offers the following options to access your id card:") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header
			font.italic: true
			horizontalAlignment: Text.AlignHCenter
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}

	Rectangle {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.95
		height: methodNfcSection.height
		color: Constants.white
		border.width: 3
		border.color: Style.color.tutorial_how

		MouseArea {
			anchors.fill: parent
			onClicked: firePush(readerMethodNfc)
		}

		Column {
			id: methodNfcSection
			width: parent.width
			spacing: Constants.component_spacing
			padding: 10

			GText {
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.9
				//: LABEL ANDROID IOS
				text: qsTr("Direct connection via NFC chip") + SettingsModel.translationTrigger
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
					text: "1"
					textStyle: Style.text.tutorial_header_secondary
					font.bold: true
					horizontalAlignment: Text.AlignHCenter
				}
			}

			Image {
				source: "qrc:///images/tutorial/generated/how_method_nfc.svg"
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.8
				height: width * (sourceSize.height / sourceSize.width)
				fillMode: Image.PreserveAspectFit
			}

			GText {
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.6
				//: LABEL ANDROID IOS
				text: (Constants.is_layout_ios ? qsTr("App on iPhone <b>with</b> NFC chip as card reader")
												: qsTr("App on Android smartphone <b>with</b> NFC chip as card reader"))
												+ SettingsModel.translationTrigger
				textStyle: Style.text.tutorial_content
				horizontalAlignment: Text.AlignHCenter
			}

			Image {
				source: "qrc:///images/tutorial/arrow_blue.svg"
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.2
				height: width * (sourceSize.height / sourceSize.width)
				fillMode: Image.PreserveAspectFit
			}
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}

	Rectangle {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.95
		height: methodSacDesktopSection.height
		color: Constants.white
		border.width: 3
		border.color: Style.color.tutorial_how

		MouseArea {
			anchors.fill: parent
			onClicked: firePush(readerMethodSacDesktop)
		}

		Column {
			id: methodSacDesktopSection
			width: parent.width
			spacing: Constants.component_spacing
			padding: 10

			GText {
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.9
				//: LABEL ANDROID IOS
				text: qsTr("Smartphone as card reader") + SettingsModel.translationTrigger
				textStyle: Style.text.tutorial_header_secondary
				font.bold: true
				horizontalAlignment: Text.AlignHCenter
			}

			Rectangle {
				anchors.horizontalCenter: parent.horizontalCenter
				height: radius * 2
				width: radius * 2
				radius: numberTwo.height
				border.width: 3
				border.color: Style.color.tutorial_how

				GText {
					id: numberTwo
					anchors.centerIn: parent
					text: "2"
					textStyle: Style.text.tutorial_header_secondary
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
					id: sacStationaryImage
					source: "qrc:///images/tutorial/generated/how_method_sac_desktop.svg"
					width: parent.width

					centerX: 0.5
					centerY: 0.5
				}

				GText {
					width: parent.width * 0.4
					//: LABEL ANDROID IOS
					text: qsTr("App on computer <b>without</b> NFC chip") + SettingsModel.translationTrigger
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

			Image {
				source: "qrc:///images/tutorial/arrow_blue.svg"
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.2
				height: width * (sourceSize.height / sourceSize.width)
				fillMode: Image.PreserveAspectFit
			}
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}

	Rectangle {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.95
		height: methodSacMobileSection.height
		color: Constants.white
		border.width: 3
		border.color: Style.color.tutorial_how

		MouseArea {
			anchors.fill: parent
			onClicked: firePush(readerMethodSacMobile)
		}

		Column {
			id: methodSacMobileSection
			width: parent.width
			spacing: Constants.component_spacing
			padding: 10

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

			Image {
				source: "qrc:///images/tutorial/arrow_blue.svg"
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.2
				height: width * (sourceSize.height / sourceSize.width)
				fillMode: Image.PreserveAspectFit
			}
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}

	Rectangle {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.95
		height: methodBluetoothSection.height
		color: Constants.white
		border.width: 3
		border.color: Style.color.tutorial_how

		MouseArea {
			anchors.fill: parent
			onClicked: firePush(readerMethodBluetooth)
		}

		Column {
			id: methodBluetoothSection
			width: parent.width
			spacing: Constants.component_spacing
			padding: 10

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

			Image {
				source: "qrc:///images/tutorial/arrow_blue.svg"
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.2
				height: width * (sourceSize.height / sourceSize.width)
				fillMode: Image.PreserveAspectFit
			}
		}
	}

	Rectangle {
		property alias text: textContent.text
		height: textContent.height + 2 * Constants.component_spacing
		width: parent.width
		color: Style.color.tutorial_how

		GText {
			id: textContent
			anchors.centerIn: parent
			color: Constants.white
			width: parent.width * 0.8
			textStyle: Style.text.tutorial_header
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("Another tip") + SettingsModel.translationTrigger
		}
	}


	Column {
		width: parent.width
		spacing: Constants.component_spacing

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("For lenghty forms, e.g. a BAf\u00F6G application, we recommend you to use the AusweisApp2 on a computer...") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header_secondary
			horizontalAlignment: Text.AlignHCenter
		}

		Item {
			width: parent.width
			height: formImage.height

			TutorialImage {
				id: formImage
				source: "qrc:///images/tutorial/generated/how_form_no_fun.svg"
				width: parent.width * 0.6

				centerX: 0.4
				centerY: 0.5
			}

			GText {
				width: parent.width * 0.5
				//: LABEL ANDROID IOS
				text: qsTr("Filling long forms is no fun on a smartphone!") + SettingsModel.translationTrigger
				textStyle: Style.text.tutorial_content
				horizontalAlignment: Text.AlignLeft
				color: Style.color.accent

				x: parent.width * 0.5
				y: (parent.height * 0.5) - (height / 2)
			}
		}

		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			//: LABEL ANDROID IOS
			text: qsTr("... and to use a smartphone to communicate with your ID card. A USB reader is of course also an alternative.") + SettingsModel.translationTrigger
			textStyle: Style.text.tutorial_header_secondary
			horizontalAlignment: Text.AlignHCenter
		}
	}

	Image {
		anchors.horizontalCenter: parent.horizontalCenter
		source: "qrc:///images/tutorial/generated/how_desktop.svg"
		width: parent.width
		height: width * (sourceSize.height / sourceSize.width)
		fillMode: Image.PreserveAspectFit
	}
}
