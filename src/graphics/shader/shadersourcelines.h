#ifndef _GTE_SHADER_SOURCE_LINES_H_
#define _GTE_SHADER_SOURCE_LINES_H_

#include <string>
#include <vector>

#include "engine.h"


namespace GTE {
    class ShaderSourceLines {
        std::vector <std::string> lines;

    public:

        class Iterator {
            friend class ShaderSourceLines;

            UInt32 index;
            const ShaderSourceLines* sourceLines;

            Iterator(UInt32 index, const ShaderSourceLines* sourceLines) : index(index), sourceLines(sourceLines) {

            }

        public:

            const Iterator& operator ++() {
                index++;
                return *this;
            }

            const std::string& operator *() {
                UInt32 trueIndex = index;
                if (trueIndex >= sourceLines->lines.size())trueIndex = 0;
                return sourceLines->lines[trueIndex];
            }

            Bool operator ==(const Iterator& other) const {
                return other.index == index;
            }

            Bool operator !=(const Iterator& other) const {
                return other.index != index;
            }

        };

        Iterator Begin() const;
        Iterator End() const;
        ShaderSourceLines();
        void Clear();
        void AddLine(std::string& line);
        UInt32 GetLineCount();
    };
}

#endif
