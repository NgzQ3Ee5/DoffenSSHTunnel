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

#endif // UTILS_H
