/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0

Item {
	id: baseItem

	property alias providerModelItem: provider.modelItem
	signal showDetailView(var pModelItem)

	height: Math.floor(width * 0.84) // Set a fixed aspect ratio which best fits the view

	Keys.onSpacePressed: mouseArea.clicked(undefined)
	Accessible.role: Accessible.Button
	Accessible.name: qsTr("Provider:") + " " + (!!providerModelItem ? provider.shortName : qsTr("Unknown error")) + SettingsModel.translationTrigger
	Accessible.description: qsTr("Provider description:") + " " + (!!providerModelItem ? provider.shortDescription : qsTr("Unknown error")) + SettingsModel.translationTrigger

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

			layer.enabled: true
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

			color: Constants.white

			GText {
				id: nameText

				anchors.fill: parent
				anchors.leftMargin: Constants.text_spacing
				anchors.rightMargin: Constants.text_spacing

				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter

				text: provider.longName !== "" ? provider.longName : provider.shortName

				elide: Text.ElideRight
				maximumLineCount: 2
				textStyle: Style.text.normal_inverse
			}
		}

		RoundedRectangle {
			width: baseItem.width
			height: Math.floor(baseItem.width * 0.08)

			radius: Style.dimens.corner_radius
			color: Category.displayColor(provider.category)
			topLeftCorner: false
			topRightCorner: false

			GText {
				id: providerText

				anchors.fill: parent
				anchors.leftMargin: Constants.text_spacing
				anchors.rightMargin: Constants.text_spacing

				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter

				text: provider.homepageBase

				elide: Text.ElideRight
				maximumLineCount: 1
				textStyle: Style.text.hint
			}
		}
	}

	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: baseItem.showDetailView(providerModelItem)
		cursorShape: Qt.PointingHandCursor
	}

	FocusFrame {
		dynamic: false
		marginFactor: 2
		radius: Style.dimens.corner_radius
		border.color: Constants.black
	}
}
