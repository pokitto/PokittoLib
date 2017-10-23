#include <SFML/Graphics.hpp>
#include "Pokitto.h"

int main()
{
    // Initialize the main window
    sf::RenderWindow app(sf::VideoMode(800, 600), "Pokitto emulator");

    // Run the setup function
    setup();

	// Start the game loop
    while (app.isOpen())
    {
        // Process events
        sf::Event event;
        while (app.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                app.close();
        }

        // Clear screen
        app.clear();

        // Draw the sprite
        app.draw(sprite);

        // Update the window
        app.display();
    }

    return EXIT_SUCCESS;
}
