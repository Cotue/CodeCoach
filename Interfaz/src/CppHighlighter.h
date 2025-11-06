//
// Created by golli on 29/10/2025.
//

#ifndef MIINTERFAZQT_CPPHIGHLIGHTER_H
#define MIINTERFAZQT_CPPHIGHLIGHTER_H

#endif //MIINTERFAZQT_CPPHIGHLIGHTER_H
#ifndef CPPHIGHLIGHTER_H
#define CPPHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QBrush>
#include <QColor>

class CppHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit CppHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> rules;

    QTextCharFormat keywordFormat;
    QTextCharFormat typeFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat numberFormat;
};

#endif // CPPHIGHLIGHTER_H
