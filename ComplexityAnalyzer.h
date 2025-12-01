//
// Created by lavar on 1/12/2025.
//

#ifndef MIINTERFAZQT_COMPLEXITYANALYZER_H
#define MIINTERFAZQT_COMPLEXITYANALYZER_H


#include <QString>

class ComplexityAnalyzer
{
public:
    struct Result {
        QString timeComplexity;
        QString spaceComplexity;

        int forLoops = 0;
        int nestedLoops = 0;
        int whileLoops = 0;
        int recursionCount = 0;
        int dynamicAllocations = 0;
        int vectorAllocations = 0;
    };

    static Result analyze(const QString &code);
};

#endif //MIINTERFAZQT_COMPLEXITYANALYZER_H