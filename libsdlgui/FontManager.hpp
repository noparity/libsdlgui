#ifndef FONTMANAGER_HPP
#define FONTMANAGER_HPP

#include <boost/filesystem.hpp>
#include "Font.hpp"
#include "SdlHelpers.hpp"
#include <unordered_map>

class FontManager
{
private:
	using FontPtr = std::unique_ptr<Font>;
	using FontCacheItem = std::pair<std::string, FontPtr>;
	using FontCache = std::unordered_map<std::string, FontPtr>;
	TTFInit m_init;
	boost::filesystem::path m_fonts;
	FontCache m_cache;

	FontManager(boost::filesystem::path fontsLocation);
	~FontManager();

	static FontManager* s_instance;

public:
	static void Destroy();
	static FontManager* GetInstance()
	{
		assert(s_instance != nullptr);
		return s_instance;
	}
	static void Initialize();
	Font* GetOrLoadFont(const std::string& name, uint8_t size, Font::Attributes attributes = Font::Attributes::Normal);
};

#endif // FONTMANAGER_HPP
