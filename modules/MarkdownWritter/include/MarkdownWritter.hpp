#include "IWritter.hpp"

#ifndef MARKDOWNWRITTER_HPP_
#define MARKDOWNWRITTER_HPP_

class MarkdownWritter : public IWritter{
    public:
        MarkdownWritter()  : _config() {}
        virtual void write(AstPtr ast) final;
        virtual void writeUserStories(Buffer &buffer, AstPtr ast) final;
        virtual void writeString(Buffer &buffer, AstPtr ast) final;
        virtual void writeDoD(Buffer &buffer, AstPtr ast) final;
        virtual void writeBullets(Buffer &buffer, AstPtr ast) final;
        virtual void writeAssignmentTable(Buffer &buffer, AstPtr ast) final;
        virtual void create(const Config::Object &data) final;
        virtual void writeDeliverablesMap(Buffer &buffer, AstPtr ast) final;
        [[nodiscard]] virtual const std::string_view getName() const noexcept final;
    private :
        Config::Object _config;
};

#endif /* !MARKDOWNWRITTER_HPP_ */
