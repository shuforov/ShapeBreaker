#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    std::cout << "Hello, ShapeBreaker!" << std::endl;

    sf::RenderWindow window(sf::VideoMode(800, 600), "ShapeBreaker");
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        window.display();
    }
    return 0;
}
