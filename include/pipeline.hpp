#pragma once

#include <string>
#include <vector>

namespace engine
{
    class pipeline
    {
        private:
            static std::vector<char> readFile(const std::string& filepath);
            void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
        public:
            pipeline(const std::string& vertFilepath, const std::string& fragFilepath);
    };
    
} // namespace engine