/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

ShaderEffect {
	readonly property color color: colorizationColor
	property color colorizationColor

	fragmentShader: "qrc:/shader/ColorOverlayShader.frag"
}
