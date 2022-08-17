/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Window 2.10
import QtQuick.Layouts 1.15

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
	Accessible.name: nameText.text

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
			fillMode: Image.PreserveAspectFit


			layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
			layer.effect: ShaderEffect {
				property var maskSource: ShaderEffectSource {
					 width: image.width
					 height: image.height

					 sourceItem: RoundedRectangle {
							width: image.width
							height: image.height

							radius: Style.dimens.corner_radius
							bottomLeftCorner: false
							bottomRightCorner: false
						}
				}

				fragmentShader: "qrc:/shader/OpacityMaskShader.frag"
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
}
