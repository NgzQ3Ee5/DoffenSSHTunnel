#ifndef UTILS_H
#define UTILS_H

#include <QRegularExpression>

namespace SanitizationUtils {

    // This is compiled once and accessible project-wide
    inline const QRegularExpression& unsafeCharsRegex()
    {
        static const QRegularExpression re(R"([^a-zA-Z0-9 \t\-_.,:;!?()@])");
        return re;
    }

} // namespace SanitizationUtils

namespace MatchUtils {

    // Matches if all words in `search` exist in `target` (AND behavior)
    inline bool matchesAllWords(const QString& target, const QString& search)
    {
        const QStringList words = search.split(' ', Qt::SkipEmptyParts);
        if (words.isEmpty()) {
            return false;
        }

        for (const QString& word : words) {
            if (!target.contains(word, Qt::CaseInsensitive)) {
                return false;
            }
        }
        return true;
    }

} // namespace MatchUtils

namespace StringUtils {

    // Keeps the left side of the string, trims from the right. Example: COPROD-Conten…
    inline QString naturalLeftTrim(const QString& input, int maxLength = 60)
    {
        if (input.length() <= maxLength)
            return input;

        int end = maxLength;
        static const QString delimiters = "/\\_.- ";
        int naturalCut = -1;

        for (int i = end; i >= 0; --i) {
            if (delimiters.contains(input[i]))
                naturalCut = i;
            if (end - i >= 10 && naturalCut != -1)
                break;
        }

        if (naturalCut == -1)
            naturalCut = end;

        return input.left(naturalCut) + QStringLiteral("…");
    }
}

#endif // UTILS_H
