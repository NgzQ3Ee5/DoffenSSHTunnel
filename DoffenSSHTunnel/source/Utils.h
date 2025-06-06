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

#endif // UTILS_H
