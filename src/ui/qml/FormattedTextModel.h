/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

class test_FormattedTextModel;

namespace governikus
{

class FormattedTextModel
	: public QAbstractListModel
{
	Q_OBJECT
	QML_UNCREATABLE("Used by ReleaseInformationModel only")
	Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
	QML_ELEMENT
	friend class ::test_FormattedTextModel;

	public:
		enum class LineType
		{
			EMPTY,
			HEADER,
			SECTION,
			SUBSECTION,
			REGULARTEXT,
			LISTITEM
		};
		Q_ENUM(LineType)

		enum FormattedTextRoles
		{
			ContentRole = Qt::UserRole + 1,
			TypeRole
		};

		explicit FormattedTextModel(QObject* pParent, const QStringList& pLines = QStringList());

		[[nodiscard]] int rowCount(const QModelIndex& pIndex = QModelIndex()) const override;
		[[nodiscard]] QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		[[nodiscard]] QHash<int, QByteArray> roleNames() const override;

		[[nodiscard]] Q_INVOKABLE bool loadSeveral(const QStringList& pFilepaths);

	private:
		struct ReadLinesResult
		{
			bool mSuccess;
			QStringList mLines;
		};

		static bool isFormattingLine(LineType pType);
		static LineType lineType(const QString& pLine);
		static QString stripFormattingCharacters(const QString& pLine, LineType pType);
		static QString replaceControlCharactersWithHtml(const QString& pLine);
		static ReadLinesResult readLines(const QString& pFilepath);

	private:
		QList<QPair<QString, LineType>> mLines;

		void processLines(const QStringList& pLines);
		void processLine(const QString& pLine);
		[[nodiscard]] bool lastLineIsEmpty() const;
};

} // namespace governikus
