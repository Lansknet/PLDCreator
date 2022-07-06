#include "IWritter.hpp"

class HTMLWritter : public IWritter
{
    public:
        virtual void write(buffer &buffer, AstPtr ast) final;
        virtual void writeUserStories(buffer &buffer, AstPtr ast) final;
        virtual void writeString(buffer &buffer, AstPtr ast) final;
        virtual void writeDoD(buffer &buffer, AstPtr ast) final;
        virtual void writeBullets(buffer &buffer, AstPtr ast) final;
};
