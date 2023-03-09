/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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

	Accessible.name: nameText.text
	Accessible.role: Accessible.Button
	height: Math.floor(width * 0.84) // Set a fixed aspect ratio which best fits the view

	Keys.onSpacePressed: mouseArea.clicked(undefined)

	ProviderModelItem {
		id: provider
	}
	Column {
		id: column
		width: baseItem.width

		Image {
			id: image
			asynchronous: true
			fillMode: Image.PreserveAspectFit
			height: Math.floor(baseItem.width * 0.56) // Image aspect ratio 16:9
			layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
			mipmap: true
			source: provider.image
			// Set a fixed size for width and height, so it doesn't have to resize the source when the window size changes -> way faster
			sourceSize.width: Screen.devicePixelRatio * 512
			width: baseItem.width

			layer.effect: ShaderEffect {
				property var maskSource: ShaderEffectSource {
					height: image.height
					width: image.width

					sourceItem: RoundedRectangle {
						bottomLeftCorner: false
						bottomRightCorner: false
						height: image.height
						radius: Style.dimens.corner_radius
						width: image.width
					}
				}

				fragmentShader: "qrc:/shader/OpacityMaskShader.frag"
			}
		}
		Rectangle {
			color: Style.color.background_pane
			height: Math.floor(baseItem.width * 0.2)
			width: baseItem.width

			GText {
				id: nameText
				anchors.fill: parent
				anchors.leftMargin: Constants.text_spacing
				anchors.rightMargin: Constants.text_spacing
				elide: Text.ElideRight
				horizontalAlignment: Text.AlignHCenter
				maximumLineCount: 2
				text: provider.longName
				textStyle: Style.text.normal
				verticalAlignment: Text.AlignVCenter
			}
		}
		RoundedRectangle {
			color: Style.currentTheme.highContrast ? Style.color.background_pane : Category.displayColor(provider.category)
			height: Math.floor(baseItem.width * 0.08)
			radius: Style.dimens.corner_radius
			topLeftCorner: false
			topRightCorner: false
			width: baseItem.width

			GSeparator {
				color: Style.color.high_contrast_item_border
				height: Style.dimens.high_contrast_item_border

				anchors {
					left: parent.left
					right: parent.right
					top: parent.top
				}
			}
		}
	}
	RoundedRectangle {
		anchors.fill: column
		borderColor: Style.color.border
		borderWidth: ApplicationModel.scaleFactor * 1
		color: Style.color.transparent
	}
	MouseArea {
		id: mouseArea
		anchors.fill: parent
		cursorShape: Qt.PointingHandCursor

		onClicked: baseItem.showDetailView(providerModelItem)
	}
}
