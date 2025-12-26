#ifndef UTILS_H
#define UTILS_H

#include <QRegularExpression>
#include <QProcess>

namespace SanitizationUtils {

    // This is compiled once and accessible project-wide
    inline const QRegularExpression& unsafeCharsRegex()
    {
        static const QRegularExpression re(R"([^a-zA-Z0-9 \t\-_.,:;!?()@])");
        return re;
    }

    // Static regexes (only compiled once)
    inline const QRegularExpression kWsUnderscores{R"([\s_]+)"};
    inline const QRegularExpression kNonAlnumDash{R"([^a-z0-9-])"};
    inline const QRegularExpression kMultiDash{R"(-{2,})"};

    // Core implementation: slugify a single string
    inline QString slugify(const QString &input) {
        // 1) Trim + lowercase
        QString s = input.trimmed().toLower();

        // 2) Normalize (NFD) and strip diacritics
        s = s.normalized(QString::NormalizationForm_D);
        QString ascii;
        ascii.reserve(s.size());
        const int n = s.size();
        for (int i = 0; i < n; ++i) {
            const QChar c = s.at(i);
            if (!c.isMark()) ascii.append(c);
        }

        // 3) Regex replacements
        ascii.replace(kWsUnderscores, "-");   // spaces/underscores -> '-'
        ascii.replace(kNonAlnumDash, "-");    // anything not [a-z0-9-] -> '-'
        ascii.replace(kMultiDash, "-");       // collapse multiple '-'

        // 4) Trim leading/trailing '-'
        if (!ascii.isEmpty() && ascii.front() == QLatin1Char('-')) ascii.remove(0, 1);
        if (!ascii.isEmpty() && ascii.back()  == QLatin1Char('-')) ascii.chop(1);

        // 5) Limit length (safe for filesystem/URLs)
        constexpr int kMaxLen = 64;
        if (ascii.size() > kMaxLen) {
            ascii.truncate(kMaxLen);
            if (!ascii.isEmpty() && ascii.back() == QLatin1Char('-')) ascii.chop(1);
        }

        // 6) Fallback if empty
        return ascii.isEmpty() ? QStringLiteral("unnamed") : ascii;
    }

    // Overload: slugify multiple inputs
    inline QString slugify(const QStringList &inputs) {
        QString combined = inputs.join(" ");
        return slugify(combined);
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

namespace ProcessUtils {
    /**
     * Checks whether a process is running.
     * Substring / command-line match.
     *
     * @param processName Name or substring to search for
     */
    inline bool isProcessRunning(const QString& processName)
    {
        if (processName.isEmpty()) {
            return false;
        }

    #ifdef Q_OS_WIN
        QProcess p;
        p.start("tasklist");
        if (!p.waitForFinished(1500)) {
            p.kill();
            return false;
        }

        const QString output = QString::fromLocal8Bit(p.readAllStandardOutput());
        return output.contains(processName, Qt::CaseInsensitive);

    #elif defined(Q_OS_MACOS) || defined(Q_OS_LINUX)
        QProcess p;
        p.start("pgrep", { "-f", processName }); // Substring match on full command line

        if (!p.waitForFinished(1500)) {
            p.kill();
            return false;
        }

        return p.exitCode() == 0;

    #else
        return false;
    #endif
    }
}

#endif // UTILS_H
