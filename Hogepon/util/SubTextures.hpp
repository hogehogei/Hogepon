#pragma once

#include <Siv3D.hpp>

class SubTextures
{
public:

    SubTextures();
    ~SubTextures() noexcept;

    bool ReadSetting(const s3d::String& filepath);
    s3d::TextureRegion SubTexture(const s3d::String& identifier_name);

private:

    void parseSubTextureSettings(const s3d::XMLElement& elem);
    void parseFileName(const s3d::XMLElement& elem);
    void parseSubTexCoords(const s3d::XMLElement& elem);

    bool m_IsSettingCompleted;
    s3d::Texture m_Texture;
    std::map<s3d::String, s3d::Rect> m_SubTexCoords;
};
