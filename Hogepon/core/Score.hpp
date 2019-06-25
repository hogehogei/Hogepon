#pragma once

#include <Siv3D.hpp>
#include <vector>

class DoujikeshiScore
{
public:

    DoujikeshiScore(const s3d::String& filepath);

    int Score(int doujikeshi_count) const;

private:

    void parseScoreTable(const s3d::XMLElement& elem);

    std::vector<int> m_ScoreTable;
};

class ChainScore
{
public:

    ChainScore(const s3d::String& filepath);

    int Score(int chain_count) const;

private:

    void parseScoreTable(const s3d::XMLElement& elem);

    std::vector<int> m_ScoreTable;
};