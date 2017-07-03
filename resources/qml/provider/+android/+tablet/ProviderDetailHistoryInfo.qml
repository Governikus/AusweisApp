import QtQuick 2.6

import ".."
import "../global"


Rectangle {
	id: baseItem

	property string providerName: ""
	property string providerPostalAddress: ""
	property string purposeText: ""
	property string requestedDataText: ""
	property string termsOfUsageText: ""
	property string internalState: "off"

	readonly property int infoMargin: Utils.dp(16)
	readonly property int infoPadding: infoMargin / 2
	readonly property int infoSpacing: Utils.dp(16)
	readonly property int leftInfoRowHeight: Utils.dp(60)

	color: "transparent"

	Rectangle {
		anchors.fill: baseItem

		color: "black"
		opacity: 0.4
	}

	Row {
		id: infoRow

		height: baseItem.height - 2 * baseItem.infoMargin
		width: baseItem.width - 2 * baseItem.infoMargin - baseItem.infoSpacing
		anchors.top: baseItem.top
		anchors.topMargin: baseItem.infoMargin
		anchors.left: baseItem.left
		anchors.leftMargin: baseItem.infoMargin

		spacing: baseItem.infoSpacing

		Rectangle {
			id: leftInfo

			height: infoRow.height
			width: infoRow.width * 2 / 5
			radius: Utils.dp(5)

			anchors.top: parent.top

			color: "white"

			Column {
				id: leftInfoColumn

				height: parent.height
				width: parent.width - 2 * baseItem.infoMargin

				padding: baseItem.infoPadding
				spacing: baseItem.infoSpacing
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.leftMargin: baseItem.infoMargin

				Row {
					height: baseItem.leftInfoRowHeight
					width: parent.width
					anchors.left: parent.left

					Image {
						id: purposeIcon

						anchors.verticalCenter: parent.verticalCenter
						height: parent.height
						width: height

						source: "qrc:///images/iOS/ProviderPurpose.png"
						fillMode: Image.PreserveAspectFit
					}

					LabeledText {
						id: purposeItem

						label: qsTr("Purpose for reading out requested data")
						text: baseItem.purposeText

						anchors.verticalCenter: parent.verticalCenter
						width: parent.width - purposeIcon.width
					}
				}

				Row {
					height: baseItem.leftInfoRowHeight
					width: parent.width
					anchors.left: parent.left

					Image {
						id: informationIcon

						anchors.verticalCenter: parent.verticalCenter
						height: parent.height
						width: height

						source: "qrc:///images/iOS/ProviderInformation.png"
						fillMode: Image.PreserveAspectFit
					}

					LabeledText {
						id: informationItem

						label: qsTr("Provider information")
						text: baseItem.providerName

						anchors.verticalCenter: parent.verticalCenter
						width: parent.width - informationIcon.width
					}
				}

				Text {
					id: readDataTitle

					width: parent.width
					anchors.left: parent.left
					font.pixelSize: Constants.header_font_size
					color: PlatformConstants.blue_primary

					text: qsTr("Read data")
				}

				Flickable {
					id: readData

					height: leftInfoColumn.height -
							(readDataTitle.height + 2 * leftInfoColumn.padding +
													3 * leftInfoColumn.spacing +
													2 * baseItem.leftInfoRowHeight)
					width: leftInfoColumn.width

					anchors.left: parent.left

					contentHeight: readDataContent.height
					contentWidth: width
					clip: true
					flickableDirection: Flickable.VerticalFlick
					boundsBehavior: Flickable.StopAtBounds

					Item {
						id: readDataContent

						height: infoTable.height
						width: readData.width

						Rectangle {
							anchors.fill: infoTable

							color: "black"
						}

						Column {
							id: infoTable

							width: parent.width
							anchors.left: parent.left
							spacing: 1

							Repeater {
								model: baseItem.requestedDataText.split(",")

								Item {
									id: textItem

									height: Utils.dp(32)
									width: infoTable.width

									Rectangle {
										anchors.fill: textItem
										color: "white"
									}

									Text {
										text: modelData.trim()

										anchors.verticalCenter: parent.verticalCenter
										font.pixelSize: Constants.normal_font_size
									}
								}
							}
						}
					}
				}
			}
		}

		Rectangle {
			id: rightInfo

			height: infoRow.height
			width: infoRow.width * 3 / 5
			radius: Utils.dp(5)

			anchors.top: parent.top

			color: "white"

			Column {
				id: termsOfUsage

				height: parent.height
				width: parent.width - 2 * baseItem.infoMargin

				padding: baseItem.infoPadding
				spacing: baseItem.infoSpacing
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.leftMargin: baseItem.infoMargin

				Text {
					id: termsOfUsageTitle

					text: qsTr("Terms of usage")
					font.pixelSize: Constants.header_font_size
					color: PlatformConstants.blue_primary
				}


				Flickable {
					id: termsOfUsageData

					height: termsOfUsage.height -
					(termsOfUsageTitle.height + 2 * termsOfUsage.padding + leftInfoColumn.spacing)
					width: termsOfUsage.width

					anchors.left: parent.left

					contentHeight: termsOfUsageTextItem.height
					contentWidth: width
					clip: true
					flickableDirection: Flickable.VerticalFlick
					boundsBehavior: Flickable.StopAtBounds

					Text {
						id: termsOfUsageTextItem

						text: baseItem.termsOfUsageText
						width: parent.width
						elide: Text.ElideRight
						wrapMode: Text.Wrap
						font.pixelSize: Constants.normal_font_size
					}
				}
			}
		}
	}
}
