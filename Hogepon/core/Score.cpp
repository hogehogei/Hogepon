
#include "core/Score.hpp"


DoujikeshiScore::DoujikeshiScore(const s3d::String& filepath)
    : m_ScoreTable()
{
    s3d::XMLReader xml(filepath);

    for (auto book = xml.firstChild(); book; book = book.nextSibling()) {
        if (book.name() == s3d::String(U"DoujikeshiScore")) {
            parseScoreTable(book);
        }
    }
}

void DoujikeshiScore::parseScoreTable(const s3d::XMLElement& elem)
{
    int table_size = s3d::Parse<int>(elem.attribute(U"TableMax").value_or(U"0"));
    m_ScoreTable.resize(table_size + 1);

    auto attributes = elem.attributes();
    for (auto itr = attributes.begin(); itr != attributes.end(); ++itr) {
        if (itr->first != U"TableMax") {
            int count = s3d::Parse<int>(itr->first.substr(1, std::string::npos));
            int score = s3d::Parse<int>(itr->second);

            m_ScoreTable[count] = score;
        }
    }
}

int DoujikeshiScore::Score(int doujikeshi_count) const
{
    if (doujikeshi_count < 4) {
        return 0;
    }

    int table_max = m_ScoreTable.size() - 1;
    if (doujikeshi_count > table_max) {
        return m_ScoreTable[table_max];
    }

    return m_ScoreTable[doujikeshi_count];
}

ChainScore::ChainScore(const s3d::String& filepath)
    : m_ScoreTable()
{
    s3d::XMLReader xml(filepath);

    for (auto book = xml.firstChild(); book; book = book.nextSibling()) {
        if (book.name() == s3d::String(U"ChainScore")) {
            parseScoreTable(book);
        }
    }
}

void ChainScore::parseScoreTable(const s3d::XMLElement& elem)
{
    int table_size = s3d::Parse<int>(elem.attribute(U"TableMax").value_or(U"0"));
    m_ScoreTable.resize(table_size + 1);

    auto attributes = elem.attributes();
    for (auto itr = attributes.begin(); itr != attributes.end(); ++itr) {
        if (itr->first != U"TableMax") {
            int count = s3d::Parse<int>(itr->first.substr(1, std::string::npos));
            int score = s3d::Parse<int>(itr->second);

            m_ScoreTable[count] = score;
        }
    }
}

int ChainScore::Score(int chain_count) const
{
    if (chain_count < 2) {
        return 0;
    }

    int table_max = m_ScoreTable.size() - 1;
    if (chain_count > table_max) {
        return m_ScoreTable[table_max];
    }

    return m_ScoreTable[chain_count];
}