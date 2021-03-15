/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Window 2.10
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: baseItem

	property alias providerModelItem: provider.modelItem
	signal showDetailView(var pModelItem)

	height: Math.floor(width * 0.84) // Set a fixed aspect ratio which best fits the view

	Keys.onSpacePressed: mouseArea.clicked(undefined)
	Accessible.role: Accessible.Button
	Accessible.name: qsTr("Provider:") + " " + (!!providerModelItem ? provider.shortName :
					 qsTr("Unknown error"))
	Accessible.description: qsTr("Provider description:") + " " + (!!providerModelItem ? provider.shortDescription :
							qsTr("Unknown error"))

	ProviderModelItem {
		id: provider
	}

	Column {
		id: column

		width: baseItem.width

		Image {
			id: image

			height: Math.floor(baseItem.width * 0.56) // Image aspect ratio 16:9
			width: baseItem.width

			source: provider.image
			// Set a fixed size for width and height, so it doesn't have to resize the source when the window size changes -> way faster
			sourceSize.width: Screen.devicePixelRatio * 512
			mipmap: true
			asynchronous: true
			fillMode: Image.PreserveAspectCrop

			layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
			layer.effect: OpacityMask {
				maskSource: Item {
					width: image.width
					height: image.height
					RoundedRectangle {
						anchors.centerIn: parent
						width: image.width
						height: image.height
						radius: Style.dimens.corner_radius
						bottomLeftCorner: false
						bottomRightCorner: false
					}
				}
			}
		}

		Rectangle {
			width: baseItem.width
			height: Math.floor(baseItem.width * 0.2)

			color: Style.color.background_pane

			GText {
				id: nameText

				anchors.fill: parent
				anchors.leftMargin: Constants.text_spacing
				anchors.rightMargin: Constants.text_spacing

				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter

				text: provider.longName

				elide: Text.ElideRight
				maximumLineCount: 2
				textStyle: Style.text.normal
			}
		}

		RoundedRectangle {
			width: baseItem.width
			height: Math.floor(baseItem.width * 0.08)

			radius: Style.dimens.corner_radius
			color: Style.currentTheme.highContrast ? Style.color.background_pane : Category.displayColor(provider.category)
			topLeftCorner: false
			topRightCorner: false

			GSeparator {
				anchors {
					top: parent.top
					left: parent.left
					right: parent.right
				}

				height: Style.dimens.high_contrast_item_border
				color: Style.color.high_contrast_item_border
			}
		}
	}

	RoundedRectangle {
		anchors.fill: column

		color: Style.color.transparent
		borderColor: Style.color.border
		borderWidth: ApplicationModel.scaleFactor * 1
	}

	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: baseItem.showDetailView(providerModelItem)
		cursorShape: Qt.PointingHandCursor
	}

	FocusFrame {
		marginFactor: 2
		radius: Style.dimens.corner_radius
		borderColor: Style.color.focus_indicator
	}
}
