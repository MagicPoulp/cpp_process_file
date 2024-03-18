class CustomException : public std::exception
{
public:
    using std::exception::exception;

    explicit CustomException(const char* message)
    {
        whatMessage = message;
    }

    const char* what() const noexcept override
    {
        return whatMessage.c_str();
    }

protected:
    std::string whatMessage;
};

class ProgramArgumentsException : public CustomException
{
public:
    using CustomException::CustomException;
    ProgramArgumentsException()
    {
        whatMessage = "Error - ProgramArgumentsException";
    }
};

class FileOpenException : public CustomException
{
public:
    using CustomException::CustomException;
    FileOpenException()
    {
        whatMessage = "Error - FileOpenException";
    }
};

class FileReadException : public CustomException
{
public:
    using CustomException::CustomException;
    FileReadException()
    {
        whatMessage = "Error - FileReadException";
    }
};

class NonUtf8CharactersFoundException : public CustomException
{
public:
    using CustomException::CustomException;
    NonUtf8CharactersFoundException()
    {
        whatMessage = "Error - NonUtf8CharactersFoundException";
    }
};
