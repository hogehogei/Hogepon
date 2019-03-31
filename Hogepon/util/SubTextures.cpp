
#include "util/SubTextures.hpp"

SubTextures::SubTextures()
    : m_IsSettingCompleted(false)
{}

SubTextures::~SubTextures()
{}

bool SubTextures::ReadSetting(const s3d::String& filepath)
{
    s3d::XMLReader xml(filepath);

    for (auto book = xml.firstChild(); book; book = book.nextSibling()) {
        if (book.name() == s3d::String(U"SubTextures")) {
            parseSubTextureSettings(book);
        }
    }

    if (m_Texture && m_SubTexCoords.size() != 0) {
        m_IsSettingCompleted = true;
    }

    return m_IsSettingCompleted;
}

s3d::TextureRegion SubTextures::SubTexture(const s3d::String& identifier_name)
{
    s3d::TextureRegion region;

    auto itr = m_SubTexCoords.find(identifier_name);
    if (itr != m_SubTexCoords.end()) {
        region = m_Texture(itr->second);
    }

    return region;
}

void SubTextures::parseSubTextureSettings(const s3d::XMLElement& elem)
{
    for (auto e = elem.firstChild(); e; e = e.nextSibling()) {
        if (e.name() == s3d::String(U"FileName")) {
            parseFileName(e);
        }
        if (e.name() == s3d::String(U"SubTexCoords")) {
            parseSubTexCoords(e);
        }
    }
}

void SubTextures::parseFileName(const s3d::XMLElement& elem)
{
    s3d::String texture_filepath = elem.text();
    m_Texture = s3d::Texture(texture_filepath);
}

void SubTextures::parseSubTexCoords(const s3d::XMLElement& elem)
{
    int x = 0, y = 0;
    int w = 0, h = 0;
    s3d::String identifier;

    identifier = elem.attribute(U"Identifier").value_or(U"");
    x = s3d::Parse<int>(elem.attribute(U"X").value_or(U"0"));
    y = s3d::Parse<int>(elem.attribute(U"Y").value_or(U"0"));
    w = s3d::Parse<int>(elem.attribute(U"Width").value_or(U"0"));
    h = s3d::Parse<int>(elem.attribute(U"Height").value_or(U"0"));
   
    if (identifier != U"") {
        m_SubTexCoords[identifier] = s3d::Rect(x, y, w, h);
    }
}