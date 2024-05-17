#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <curl/curl.h>


//API get word function
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string getWordFromAPI() {
    CURL* curl = curl_easy_init();
    std::string fullWord;

    if (curl) {
        std::string apiUrl = "https://random-word-api.herokuapp.com/word?number=1&length=5";

        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fullWord);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Curl failed: " << curl_easy_strerror(res) << std::endl;
            return "";
        }

        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Curl initialization failed." << std::endl;
        return "";
    }

    std::string word = fullWord.substr(2, 5);

    return word;
}

bool isLetterInWord(char letter, const std::string& word) {
    return word.find(letter) != std::string::npos;
}


int main() {
    //SFML window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Wordle Game");
    //all varaible setup
    std::string targetWord = getWordFromAPI();

    if (targetWord.empty()) {
        std::cerr << "Error fetching word from API." << std::endl;
        return -1;
    }

    std::vector<std::string> guesses;
    std::vector<std::string> feedback;

    
    std::string userInput;
    char input;

    sf::Font font;
    if (!font.loadFromFile("C:\\Users\\undaz\\OneDrive\\Dators\\KD\\arial.ttf")) {
        std::cerr << "Error loading font file" << std::endl;
        return -1;
    }

    sf::Text guessedText;
    guessedText.setFont(font);
    guessedText.setCharacterSize(24);
    guessedText.setFillColor(sf::Color::White);
    guessedText.setPosition(10, 10);

    sf::Text inputText;
    inputText.setFont(font);
    inputText.setCharacterSize(24);
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition(10, 50);

    sf::Text feedbackText;
    feedbackText.setFont(font);
    feedbackText.setCharacterSize(24);
    feedbackText.setPosition(10, 90);

    int remainingGuesses = 7;

    sf::Text ruleText;
    ruleText.setFont(font);
    ruleText.setCharacterSize(18);
    ruleText.setPosition(10, 400);
    ruleText.setString("This is a Wordle game!\nGuess the secret word by writing any five-letter word.");

    sf::RectangleShape greenSquare(sf::Vector2f(20.0f, 20.0f));
    greenSquare.setPosition(10, 460);
    greenSquare.setFillColor(sf::Color::Green);
    sf::Text ruleTextG;
    ruleTextG.setFont(font);
    ruleTextG.setCharacterSize(18);
    ruleTextG.setPosition(35, 460);
    ruleTextG.setString("Letter is in correct place");


    sf::RectangleShape yellowSquare(sf::Vector2f(20.0f, 20.0f));
    yellowSquare.setPosition(10, 490);
    yellowSquare.setFillColor(sf::Color::Yellow);
    sf::Text ruleTextY;
    ruleTextY.setFont(font);
    ruleTextY.setCharacterSize(18);
    ruleTextY.setPosition(35, 490);
    ruleTextY.setString("Letter is in incorrect place");

    sf::RectangleShape whiteSquare(sf::Vector2f(20.0f, 20.0f));
    whiteSquare.setPosition(10, 520);
    whiteSquare.setFillColor(sf::Color::White);
    sf::Text ruleTextW;
    ruleTextW.setFont(font);
    ruleTextW.setCharacterSize(18);
    ruleTextW.setPosition(35, 520);
    ruleTextW.setString("Letter is not in word");



    while (window.isOpen() && remainingGuesses > 0 && userInput != targetWord) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    input = static_cast<char>(event.text.unicode);

                    if (input == '\b') { // Backspace key
                        if (!userInput.empty()) {
                            userInput.pop_back();
                        }
                    }
                    else {
                        userInput += input;
                    }
                }
            }

            // Check for Enter key
            if (event.key.code == sf::Keyboard::Enter) {
                //fixes whitespace at the input field after first guess
                userInput.erase(std::remove_if(userInput.begin(), userInput.end(), ::isspace), userInput.end());

                //count only letters without the whitespace
                int count = 0;
                for (char c : userInput) {
                    if (!std::isspace(static_cast<unsigned char>(c))) {
                        count++;
                    }
                }
                if (count == 5) {
                    std::string feedbackStr;
                    for (std::size_t i = 0; i < targetWord.size(); ++i) {
                        if (userInput[i] == targetWord[i]) {
                            feedbackStr += "G"; // Green
                          
                        }
                        else if (isLetterInWord(userInput[i], targetWord)) {
                            feedbackStr += "Y"; // Yellow
                           
                        }
                        else {
                            feedbackStr += "W"; // White
                            
                        }
                    }

                    guesses.push_back(userInput);
                    feedback.push_back(feedbackStr);
                    --remainingGuesses;

                    userInput.clear();
                }
            }
        }

        window.clear();

        // Display the guessed words and feedback
        std::string allGuesses;
        for (std::size_t i = 0; i < guesses.size(); ++i) {
            allGuesses += "Guess " + std::to_string(i + 1) + ": " + guesses[i] + +"\n";

            // Colored squares for feedback
            float startX = 600.0f;
            float startY = 100.0f + static_cast<float>(i) * 30.0f;

            for (char c : feedback[i]) {
                sf::RectangleShape square(sf::Vector2f(20.0f, 20.0f));
                square.setPosition(startX, startY);

                if (c == 'G') {
                    square.setFillColor(sf::Color::Green);
                }
                else if (c == 'Y') {
                    square.setFillColor(sf::Color::Yellow);
                }
                else if (c == 'W') {
                    square.setFillColor(sf::Color::White);
                }

                window.draw(square);

                startX += 30.0f;
            }

        }
        // Output
        guessedText.setString("Remaining Guesses: " + std::to_string(remainingGuesses));
        window.draw(guessedText);

        inputText.setString("Input: " + userInput);
        window.draw(inputText);

        feedbackText.setString(allGuesses);
        window.draw(feedbackText);

        window.draw(ruleText);
        window.draw(ruleTextG);
        window.draw(ruleTextY);
        window.draw(ruleTextW);

        window.draw(greenSquare);
        window.draw(yellowSquare);
        window.draw(whiteSquare);

        window.display();
    }

    window.clear();

    // End screen
    sf::Text resultText;
    resultText.setFont(font);
    resultText.setCharacterSize(24);
    resultText.setPosition(200, 200);

    if (remainingGuesses == 0) {
        resultText.setFillColor(sf::Color::Red);
        resultText.setString("Sorry, you've run out of guesses.\nThe correct word was: ");
    }
    else {
        resultText.setFillColor(sf::Color::Green);
        resultText.setString("Congratulations!\nYou've guessed the word: " );
    }

    window.clear();

    window.draw(resultText);

    float lineHeight = resultText.getGlobalBounds().height;
    resultText.move(0, lineHeight + 10);

    std::string targetWordUpper;
    for (char c : targetWord) {
        targetWordUpper += std::toupper(c);
    }

    sf::Text targetText;
    targetText.setFont(font);
    targetText.setCharacterSize(33);
    targetText.setPosition(resultText.getPosition().x, resultText.getPosition().y + lineHeight + 10);
    targetText.setFillColor(sf::Color::White);
    targetText.setString(targetWordUpper);

    window.draw(targetText);

    window.display();

    // User closes the window
    sf::Event event;
    while (window.waitEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }

    return 0;
}
