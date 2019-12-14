#include <iostream>
#include <fstream>
#include "HashMap.hpp"

/**
 * Defines the expected arguments amount.
 */
const int ARGS_AMOUNT = 4;
/**
 * Defines the program usage massage.
 */
const char* USAGE_MSG = "Usage: SpamDetector <database path> <message path> <threshold>";
/**
 * Define invalid input massage.
 */
const char* INVALID_INPUT_MSG = "Invalid input";
/**
 * Defines memory allocation erroe massage.
 */
const char* MEMORY_MSG = "Memory allocation failed";
/**
 * Defines the index of the spam file path argument.
 */
const int SPAM_FILE_ARG = 1;
/**
 * Defines the index of the massage file path argument.
 */
const int MSG_FILE_ARG = 2;
/**
 * Defines the index of the threshold argument.
 */
const int THRESHOLD_ARG = 3;
/**
 * Defines the separator in the spam file.
 */
const char SEPARATOR = ',';
/**
 * Define the spam massage to print.
 */
const char* SPAM_MSG = "SPAM";
/**
 * Define the not-spam massage to print.
 */
const char* NOT_SPAM_MSG = "NOT_SPAM";

/**
 * Parse the spam file and put the phrase with there score into the given HashMap. If the file is
 * invalid, it will throw an exception.
 * @param spamFile File to parse.
 * @param spamMap The HashMap to insert all the phrases and their score.
 */
void parseSpamFile(std::ifstream& spamFile, HashMap<std::string, int>& spamMap)
{
    std::string line, phrase, strScore;
    int score;
    while (getline(spamFile, line))
    {
        int i = 0;
        for ( ; i < (int) line.length(); i++)
        {
            if (line[i] == SEPARATOR)
            {
                break;
            }
        }
        phrase = line.substr(0, i);
        strScore = line.substr(i + 1, line.length() - i - 1);
        size_t end;
        score = std::stoi(line.substr(i + 1, line.length() - i - 1), &end);
        if (end != strScore.length())
        {
            throw std::invalid_argument(INVALID_INPUT_MSG);
        }
        spamMap.insert(phrase, score);
    }
}

/**
 * Change the given text string to lower cases.
 * @param text String to change.
 */
void toLower(std::string& text)
{
    for (unsigned long i = 0; i < text.length(); i++)
    {
        text[i] = tolower(text[i]);
    }
}

/**
 * Check for every phrase how many times it appears in the massage, and calculate the score of this
 * massage.
 * @param massageFile the massage text file.
 * @param spamMap the spam map that contains all the spam phrases.
 * @return the score of this massage.
 */
int checkSpam(std::ifstream& massageFile, const HashMap<std::string, int>& spamMap)
{
    std::string text, line;
    while (getline(massageFile, line))
    {
        text += line + " ";
    }
    toLower(text);
    int score = 0;
    for (auto it = spamMap.cbegin(); it != spamMap.cend(); it++)
    {
        std::string phrase = it->first;
        toLower(phrase);
        size_t loc = text.find(phrase);
        while (loc != std::string::npos)
        {
            score += it->second;
            loc = text.find(phrase, loc + 1);
        }
    }
    return score;
}

/**
 * Runs the whole program and print a spam/ not-spam massage.
 * @param argc The number of arguments.
 * @param argv An array of the arguments.
 * @return EXIT_SUCCESS if the program run successfully, EXIT_FAILURE otherwise.
 */
int main(int argc, char *argv[])
{
    if (argc != ARGS_AMOUNT)
    {
        std::cerr << USAGE_MSG << std::endl;
        return EXIT_FAILURE;
    }
    std::ifstream spamFile(argv[SPAM_FILE_ARG]), massageFile(argv[MSG_FILE_ARG]);
    std::string strThreshold = argv[THRESHOLD_ARG];
    size_t end;
    int threshold = std::stoi(strThreshold, &end);
    if (!spamFile || !massageFile || end != strThreshold.length() || threshold <= 0)
    {
        std::cerr << INVALID_INPUT_MSG << std::endl;
        return EXIT_FAILURE;
    }
    int score;
    try
    {
        HashMap<std::string, int> spamMap;
        parseSpamFile(spamFile, spamMap);
        score = checkSpam(massageFile, spamMap);
    }
    catch (const std::bad_alloc&)
    {
        std::cerr << MEMORY_MSG << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::invalid_argument&)
    {
        std::cerr << INVALID_INPUT_MSG << std::endl;
        return EXIT_FAILURE;
    }
    if (score >= threshold)
    {
        std::cout << SPAM_MSG << std::endl;
    }
    else
    {
        std::cout << NOT_SPAM_MSG << std::endl;
    }
    return EXIT_SUCCESS;
}

