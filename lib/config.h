#pragma once

#ifdef __cplusplus

class ConfigProto;
class OptionProto;
class FluxSource;

class OptionException : public ErrorException
{
public:
    OptionException(const std::string& message): ErrorException(message) {}
};

class OptionNotFoundException : public OptionException
{
public:
    OptionNotFoundException(const std::string& message): OptionException(message) {}
};

class InvalidOptionException : public OptionException
{
public:
    InvalidOptionException(const std::string& message): OptionException(message) {}
};

class InapplicableOptionException : public OptionException
{
public:
    InapplicableOptionException(const std::string& message): OptionException(message) {}
};

class Config
{
public:
	ConfigProto* operator -> () const;
	operator ConfigProto* () const;
	operator ConfigProto& () const;

	/* Set and get individual config keys. */

	void set(std::string key, std::string value);
	std::string get(std::string key);

	/* Reset the entire configuration. */

	void clear();

	/* Merge in one config file. */

	void readConfigFile(std::string filename);

    /* Option management: look up an option by name, determine whether an option
     * is valid, and apply an option. */

	const OptionProto& findOption(const std::string& option);
	bool isOptionValid(const OptionProto& option);
    void applyOption(const OptionProto& option);

	/* Adjust overall inputs and outputs. */

	void setFluxSource(std::string value);
	void setFluxSink(std::string value);
	void setCopyFluxTo(std::string value);
	void setImageReader(std::string value);
	void setImageWriter(std::string value);

	/* Fetch the sources, opening them if necessary. */

	bool hasFluxSource() const;
	std::shared_ptr<FluxSource>& getFluxSource();

private:
	std::shared_ptr<FluxSource> _fluxSource;
};

extern Config& globalConfig();

#endif

