#include "CppHighlighter.h"

CppHighlighter::CppHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // --- Palabras clave ---
    keywordFormat.setForeground(QColor("#569CD6"));
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        "\\bif\\b", "\\belse\\b", "\\bfor\\b", "\\bwhile\\b", "\\breturn\\b",
        "\\bclass\\b", "\\bstruct\\b", "\\bswitch\\b", "\\bcase\\b",
        "\\bpublic\\b", "\\bprivate\\b", "\\bprotected\\b",
        "\\bnamespace\\b", "\\busing\\b", "\\btry\\b", "\\bcatch\\b",
        "\\bthrow\\b", "\\bnew\\b", "\\bdelete\\b", "\\bvirtual\\b",
        "\\bconst\\b", "\\bstatic\\b", "\\binline\\b", "\\btemplate\\b"
    };

    for (const QString &pattern : keywordPatterns) {
        rules.append({ QRegularExpression(pattern), keywordFormat });
    }

    // --- Tipos de datos ---
    typeFormat.setForeground(QColor("#4EC9B0"));
    const QString typePatterns[] = {
        "\\bint\\b", "\\bfloat\\b", "\\bdouble\\b", "\\bchar\\b",
        "\\bbool\\b", "\\bvoid\\b", "\\blong\\b", "\\bshort\\b",
        "\\bunsigned\\b", "\\bsigned\\b", "\\bauto\\b", "\\bstring\\b"
    };

    for (const QString &pattern : typePatterns) {
        rules.append({ QRegularExpression(pattern), typeFormat });
    }

    // --- Números ---
    numberFormat.setForeground(QColor("#B5CEA8"));
    rules.append({ QRegularExpression("\\b[0-9]+\\b"), numberFormat });

    // --- Cadenas ---
    stringFormat.setForeground(QColor("#CE9178"));
    rules.append({ QRegularExpression("\".*\""), stringFormat });

    // --- Comentarios ---
    commentFormat.setForeground(QColor("#6A9955"));
    rules.append({ QRegularExpression("//[^\n]*"), commentFormat });
    rules.append({ QRegularExpression("/\\*.*\\*/"), commentFormat });
}

void CppHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : rules) {
        QRegularExpressionMatchIterator i = rule.pattern.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
