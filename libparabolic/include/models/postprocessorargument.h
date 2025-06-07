#ifndef POSTPROCESSORARGUMENT_H
#define POSTPROCESSORARGUMENT_H

#include <string>
#include <boost/json.hpp>
#include "executable.h"
#include "postprocessor.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of arguments to pass to a post-processor.
     */
    class PostProcessorArgument
    {
    public:
        /**
         * @brief Constructs a PostProcessorArgument.
         * @param name The name of the arguments
         * @param postProcessor The post-processor to pass the arguments too
         * @param executable The executable to pass the arguments too
         * @param args The arguments to pass to the post-processor
         */
        PostProcessorArgument(const std::string& name, PostProcessor postProcessor, Executable executable, const std::string& args);
        /**
         * @brief Constructs a PostProcessorArgument.
         * @param json The JSON object to construct the PostProcessorArgument from
         */
        PostProcessorArgument(boost::json::object json);
        /**
         * @brief Gets the name of the arguments.
         * @return The name of the arguments
         */
        const std::string& getName() const;
        /**
         * @brief Sets the name of the arguments.
         * @param name The new name of the arguments
         */
        void setName(const std::string& name);
        /**
         * @brief Gets the PostProcessor for the arguments.
         * @return The PostProcess for the arguments
         */
        PostProcessor getPostProcessor() const;
        /**
         * @brief Sets the PostProcessor for the arguments.
         * @param postProcessor The new PostProcess for the arguments
         */
        void setPostProcessor(PostProcessor postProcessor);
        /**
         * @brief Gets the Executable for the arguments.
         * @return The Executable for the arguments
         */
        Executable getExecutable() const;
        /**
         * @brief Sets the Executable for the arguments.
         * @param executable The new Executable for the arguments
         */
        void setExecutable(Executable executable);
        /**
         * @brief Gets the arguments for the post-processor.
         * @return The arguments for the post-processor
         */
        const std::string& getArgs() const;
        /**
         * @brief Sets the arguments for the post-processor.
         * @param args The new arguments for the post-processor
         */
        void setArgs(const std::string& args);
        /**
         * @brief Gets the string representation of the PostProcessArguments.
         * @return The string representation of the PostProcessArguments
         */
        std::string str() const;
        /**
         * @brief Converts the PostProcessArguments to a JSON object.
         * @return The JSON object
         */
        boost::json::object toJson() const;
        /**
         * @brief Compares two PostProcessorArguments via ==.
         * @param other The other PostProcessorArguments to compare
         * @return True if this == other
         */
        bool operator==(const PostProcessorArgument& other) const;
        /**
         * @brief Compares two PostProcessorArguments via !=.
         * @param other The other PostProcessorArguments to compare
         * @return True if this != other
         */
        bool operator!=(const PostProcessorArgument& other) const;
        /**
         * @brief Compares two PostProcessorArguments via <.
         * @param other The other PostProcessorArguments to compare
         * @return True if this < other
         */
        bool operator<(const PostProcessorArgument& other) const;
        /**
         * @brief Compares two PostProcessorArguments via >.
         * @param other The other PostProcessorArguments to compare
         * @return True if this > other
         */
        bool operator>(const PostProcessorArgument& other) const;

    private:
        std::string m_name;
        PostProcessor m_postProcessor;
        Executable m_executable;
        std::string m_args;
    };
}

#endif //POSTPROCESSORARGUMENT_H
