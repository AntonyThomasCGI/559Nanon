
#include "textmate/nanon_pattern.hpp"

Pattern::Pattern(QString pattern)
: QRegularExpression(pattern) {};
// {
//     this->matchType = type;
// }
