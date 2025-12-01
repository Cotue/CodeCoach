//
// Created by lavar on 1/12/2025.
//

#include "ComplexityAnalyzer.h"
#include <QRegularExpression>


ComplexityAnalyzer::Result ComplexityAnalyzer::analyze(const QString &code)
{
    Result r;

    // === DETECTAR LOOPS ===
    r.forLoops = code.count(QRegularExpression("\\bfor\\s*\\("));
    r.whileLoops = code.count(QRegularExpression("\\bwhile\\s*\\("));

    // Detectar bucles anidados (caso simple)
    r.nestedLoops = code.count(QRegularExpression("for\\s*\\(.*\\n.*for\\s*\\("));

    // === DETECTAR RECURSIÓN ===
    QRegularExpression funcRegex("(\\w+)\\s*\\(");
    QRegularExpressionMatchIterator it = funcRegex.globalMatch(code);

    QSet<QString> functions;
    while (it.hasNext()) {
        auto match = it.next();
        functions.insert(match.captured(1));
    }

    for (const QString &f : functions) {
        QString pattern = QString("\\b%1\\s*\\(").arg(f);
        int count = code.count(QRegularExpression(pattern));
        if (count > 1)
            r.recursionCount++;
    }

    // === ANALIZAR ESPACIO ===
    r.dynamicAllocations = code.count(QRegularExpression("\\bnew\\s+"));
    r.vectorAllocations = code.count(QRegularExpression("std::vector"));

    // === ESTIMAR COMPLEJIDAD ===

    // Temporal
    if (r.nestedLoops > 0)
        r.timeComplexity = "O(n^2)";
    else if (r.forLoops + r.whileLoops >= 1)
        r.timeComplexity = "O(n)";
    else if (r.recursionCount > 0)
        r.timeComplexity = "O(n) a O(2^n) según el caso";
    else
        r.timeComplexity = "O(1)";

    // Espacial
    if (r.vectorAllocations > 0 || r.dynamicAllocations > 0)
        r.spaceComplexity = "O(n)";
    else
        r.spaceComplexity = "O(1)";

    return r;
}
