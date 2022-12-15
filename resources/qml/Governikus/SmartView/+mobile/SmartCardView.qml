/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SmartModel 1.0

Item {
	id: root

	readonly property real padding: Constants.groupbox_spacing
	readonly property int smartState: SmartModel.smartState

	implicitHeight: cardIcon.implicitHeight + 2 * padding
	implicitWidth: cardIcon.implicitWidth + 2 * padding

	TintableIcon {
		id: cardIcon
		anchors.fill: parent
		anchors.margins: root.padding
		desaturate: true
		source: "qrc:///images/mobile/generic_id_card.svg"
		sourceSize.height: Style.dimens.header_icon_size - 2 * root.padding
		tintEnabled: root.smartState !== SmartModel.SMART_READY
	}
	TintableIcon {
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		source: "qrc:///images/material_alert.svg"
		sourceSize.width: Style.dimens.medium_icon_size
		tintColor: Style.color.warning_text
		visible: root.smartState === SmartModel.SMART_UNUSABLE
	}
	TintableIcon {
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		source: "qrc:///images/sandglass.svg"
		sourceSize.height: Style.dimens.medium_icon_size
		tintColor: Style.color.accent
		visible: root.smartState === SmartModel.SMART_UPDATING_STATUS
	}
	Canvas {
		id: canvas
		anchors.fill: parent
		visible: smartState === SmartModel.SMART_UNAVAILABLE

		onPaint: {
			var context = canvas.getContext('2d');
			if (context === null) {
				return;
			}
			context.save();
			context.reset();
			context.strokeStyle = Style.color.warning_text;
			context.lineWidth = root.padding;
			context.lineCap = "round";
			context.beginPath();
			context.moveTo(context.lineWidth, height - context.lineWidth);
			context.lineTo(width - context.lineWidth, context.lineWidth);
			context.stroke();
			context.restore();
		}
	}
}
