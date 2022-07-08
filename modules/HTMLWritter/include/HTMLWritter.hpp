#ifndef HTMLWRITTER_HPP_
#define HTMLWRITTER_HPP_

#include "Config.hpp"
#include "IWritter.hpp"

class HTMLWritter : public IWritter
{
    public:
        HTMLWritter() : _config() {}
        virtual void write(AstPtr ast) final;
        virtual void writeUserStories(Buffer &buffer, AstPtr ast) final;
        virtual void writeString(Buffer &buffer, AstPtr ast) final;
        virtual void writeDoD(Buffer &buffer, AstPtr ast) final;
        virtual void writeBullets(Buffer &buffer, AstPtr ast) final;
        virtual void create(const Config::Object &data) final;
        [[nodiscard]] virtual const std::string_view getName() const noexcept final;
    private :
        Config::Object _config;
};

#endif /* !HTMLWRITTER_HPP_ */
