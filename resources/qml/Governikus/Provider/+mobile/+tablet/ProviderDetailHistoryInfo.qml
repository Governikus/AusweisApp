/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0

Item {
	id: baseItem

	property string providerName: ""
	property string providerPostalAddress: ""
	property string purposeText: ""
	property string requestedDataText: ""
	property string termsOfUsageText: ""
	property string internalState: "off"

	Rectangle {
		anchors.fill: baseItem

		color: "black"
		opacity: 0.4
	}

	GFlickable {
		anchors.fill: baseItem
		anchors.margins: Constants.component_spacing
		contentHeight: infoRow.height

		onContentYChanged: {
			if (contentY < 0) { contentY = 0 /* prevent flicking over the top */}
		}

		Row {
			id: infoRow
			height: Math.max(leftColumn.height, rightColumn.height) + 2 * Constants.pane_padding
			spacing: Constants.component_spacing

			Item {
				height: 1
				width: baseItem.width / 3

				GPane {
					id: leftPane

					height: infoRow.height
					anchors {
						left: parent.left
						right: parent.right
					}
				}

				Column {
					id: leftColumn
					anchors.margins: Constants.pane_padding
					anchors.left: parent.left
					anchors.top: parent.top
					anchors.right: parent.right
					spacing: Constants.pane_spacing

					ProviderInfoSection {
						imageSource: "qrc:///images/provider/information.svg"
						//: LABEL ANDROID_TABLET IOS_TABLET
						title: qsTr("Provider")
						name: baseItem.providerName
					}

					ProviderInfoSection {
						imageSource: "qrc:///images/provider/purpose.svg"
						//: LABEL ANDROID_TABLET IOS_TABLET
						title: qsTr("Purpose for reading out requested data")
						name: baseItem.purposeText
					}

					GText {
						id: readDataTitle
						width: parent.width
						//: LABEL ANDROID_TABLET IOS_TABLET
						text: qsTr("Read data")
						textStyle: Style.text.header_accent
					}

					Column {
						id: infoTable

						width: parent.width
						spacing: 1

						Repeater {
							model: baseItem.requestedDataText.split(",")

							Item {
								id: textItem

								height: 32
								width: infoTable.width

								Rectangle {
									anchors.fill: textItem
									color: Constants.white
								}

								GText {
									anchors.verticalCenter: parent.verticalCenter

									text: modelData.trim()
									textStyle: Style.text.normal_secondary
								}
							}
						}
					}
				}
			}

			Item {
				height: 1
				width: baseItem.width / 3 * 2 - 3 * Constants.component_spacing

				GPane {
					id: rightPane

					height: infoRow.height
					anchors {
						left: parent.left
						right: parent.right
					}
				}

				Column {
					id: rightColumn
					anchors.margins: Constants.pane_padding
					anchors.left: parent.left
					anchors.top: parent.top
					anchors.right: parent.right
					spacing: Constants.pane_spacing

					GText {
						id: termsOfUsageTitle
						//: LABEL ANDROID_TABLET IOS_TABLET
						text: qsTr("Terms of usage")
						textStyle: Style.text.header_accent
					}

					GText {
						id: termsOfUsageTextItem

						width: parent.width

						text: baseItem.termsOfUsageText
						textStyle: Style.text.normal_secondary
					}
				}
			}
		}
	}
}
