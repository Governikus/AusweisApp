import QtQuick 2.10

import Governikus.Global 1.0

TutorialContent {
	id: baseItem

	signal firePush(var pSectionPage)
	signal quitTutorialClicked()

	TutorialReaderMethodNfc {
		id: readerMethodNfc
		visible: false
		onQuitTutorialClicked:  baseItem.quitTutorialClicked()
	}

	TutorialReaderMethodSacMobile {
		id: readerMethodSacMobile
		visible: false
		onQuitTutorialClicked:  baseItem.quitTutorialClicked()
	}

	TutorialReaderMethodSacDesktop {
		id: readerMethodSacDesktop
		visible: false
		onQuitTutorialClicked:  baseItem.quitTutorialClicked()
	}

	TutorialReaderMethodBluetooth {
		id: readerMethodBluetooth
		visible: false
		onQuitTutorialClicked:  baseItem.quitTutorialClicked()
	}

	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width * 0.9
		text: qsTr("How can I use the AusweisApp2 on my smartphone?") + settingsModel.translationTrigger
		font.family: "Noto Serif"
		font.pixelSize: Constants.tutorial_content_header_h1_font_size
		font.italic: true
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
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

			Text {
				width: parent.width * 0.6
				text: qsTr("Many Android devices can access the id card via the NFC interface.") + settingsModel.translationTrigger
				font.family: "Noto Serif"
				font.pixelSize: Constants.tutorial_content_font_size
				font.bold: true
				horizontalAlignment: Text.AlignLeft
				wrapMode: Text.WordWrap

				x: (parent.width * 0.65) - (width / 2)
				y: (parent.height * 0.5) - (height / 2)
			}
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("You can find a list of compatible NFC-capable smartphones here:") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h2_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: "<a href=\"%1\">%1</a>".arg(qsTr("https://www.ausweisapp.bund.de/mobile-geraete/")) + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
			onLinkActivated: Qt.openUrlExternally(link)
		}

		Image {
			source: "qrc:///images/tutorial/generated/how_device_lineup.svg"
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width
			height: width * (sourceSize.height / sourceSize.width)
			fillMode: Image.PreserveAspectFit
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("The AusweisApp2 offers the following options to access your id card:") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			font.italic: true
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}
	}

	TutorialSeperator {
		source: "qrc:///images/tutorial/section_seperator_how.svg"
	}

	Item {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width
		height: methodNfcSection.height

		MouseArea {
			anchors.fill: parent
			onClicked: firePush(readerMethodNfc)
		}

		Column {
			id: methodNfcSection
			width: parent.width
			spacing: Constants.component_spacing
			padding: Utils.dp(10)

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.9
				text: qsTr("Direct connection via NFC chip") + settingsModel.translationTrigger
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
				radius: numberOne.height
				border.width: Utils.dp(3)
				border.color: Constants.tutorial_blue

				Text {
					id: numberOne
					anchors.centerIn: parent
					text: "1"
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h2_font_size
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

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.width * 0.6
				text: qsTr("App on Android smartphone <b>with</b> NFC chip as card reader") + settingsModel.translationTrigger
				font.family: "Noto Serif"
				font.pixelSize: Constants.tutorial_content_font_size
				horizontalAlignment: Text.AlignHCenter
				wrapMode: Text.WordWrap
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

	Item {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width
		height: methodSacDesktopSection.height

		MouseArea {
			anchors.fill: parent
			onClicked: firePush(readerMethodSacDesktop)
		}

		Column {
			id: methodSacDesktopSection
			width: parent.width
			spacing: Constants.component_spacing
			padding: Utils.dp(10)

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

	Item {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width
		height: methodSacMobileSection.height

		MouseArea {
			anchors.fill: parent
			onClicked: firePush(readerMethodSacMobile)
		}

		Column {
			id: methodSacMobileSection
			width: parent.width
			spacing: Constants.component_spacing
			padding: Utils.dp(10)

			Rectangle {
				anchors.horizontalCenter: parent.horizontalCenter
				height: radius * 2
				width: radius * 2
				radius: numberTwo.height
				border.width: Utils.dp(3)
				border.color: Constants.tutorial_blue

				Text {
					id: numberThree
					anchors.centerIn: parent
					text: "3"
					font.family: "Noto Serif"
					font.pixelSize: Constants.tutorial_content_header_h2_font_size
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
					id: sacMobileImage
					source: "qrc:///images/tutorial/generated/how_method_sac_mobile.svg"
					width: parent.width

					centerX: 0.5
					centerY: 0.5
				}

				Text {
					width: parent.width * 0.4
					text: qsTr("App on tablet or smartphone <b>without</b> NFC chip") + settingsModel.translationTrigger
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

	Item {
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width
		height: methodBluetoothSection.height

		MouseArea {
			anchors.fill: parent
			onClicked: firePush(readerMethodBluetooth)
		}

		Column {
			id: methodBluetoothSection
			width: parent.width
			spacing: Constants.component_spacing
			padding: Utils.dp(10)

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
		color: Constants.tutorial_blue

		Text {
			id: textContent
			anchors.centerIn: parent
			color: Constants.white
			width: parent.width * 0.8
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h1_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
			text: qsTr("Another tip") + settingsModel.translationTrigger
		}
	}


	Column {
		width: parent.width
		spacing: Constants.component_spacing

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("For lenghty forms, e.g. a BAf\u00F6G application, we recommend you to use the AusweisApp2 on a computer...") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h2_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
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

			Text {
				width: parent.width * 0.5
				text: qsTr("Filling long forms is no fun on a smartphone!") + settingsModel.translationTrigger
				horizontalAlignment: Text.AlignLeft
				wrapMode: Text.WordWrap
				color: Constants.tutorial_blue

				x: parent.width * 0.5
				y: (parent.height * 0.5) - (height / 2)
			}
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9
			text: qsTr("... and to use a smartphone to communicate with your ID card. A USB reader is of course also an alternative.") + settingsModel.translationTrigger
			font.family: "Noto Serif"
			font.pixelSize: Constants.tutorial_content_header_h2_font_size
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
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
