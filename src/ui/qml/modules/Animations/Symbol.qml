/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global

TintableIcon {
	id: root

	enum Type {
		BLOCK,
		CHECK,
		ERROR,
		INFO,
		QUESTION,
		STAR,
		WARNING
	}

	property int type: Symbol.Type.STAR

	source: {
		switch (root.type) {
		case Symbol.Type.BLOCK:
			return "qrc:///animations/symbol_block.svg";
		case Symbol.Type.CHECK:
			return "qrc:///animations/symbol_check.svg";
		case Symbol.Type.ERROR:
			return "qrc:///animations/symbol_error.svg";
		case Symbol.Type.INFO:
			return "qrc:///animations/symbol_info.svg";
		case Symbol.Type.QUESTION:
			return "qrc:///animations/symbol_question.svg";
		case Symbol.Type.STAR:
			return "qrc:///animations/symbol_star.svg";
		case Symbol.Type.WARNING:
			return "qrc:///animations/symbol_warning.svg";
		default:
			return "";
		}
	}
	tintEnabled: false
}
